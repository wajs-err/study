# Scalable Scheduler

## Пререквизиты

- [cond/wait_group](/tasks/cond/wait_group)
- [fibers/mutex](/tasks/fibers/mutex)
- [tasks/executors](/tasks/tasks/executors)
- [lockfree/steal](/tasks/lockfree/steal)
- [fibers/chan](/tasks/fibers/chan) – рекомендуется

----

## Пролог

_I suspect that making the scheduler use per-CPU
queues together with some inter-CPU load balancing
logic is probably trivial. Patches already exist, and I
don’t feel that people can screw up the few hundred lines
too badly._ – Linus Torvalds

Credit: [The Linux Scheduler: a Decade of Wasted Cores](https://people.ece.ubc.ca/sasha/papers/eurosys16-final29.pdf)

----

В этой задаче вы должны реализовать [масштабируемый планировщик](exe/executors/tp/fast) для файберов.

Масштабируемость означает, что с ростом числа ядер планировщик должен выполнять пропорционально больше задач в единицу времени.

## Планировщик для файберов

До этого момента в качестве планировщика файберов мы использовали простой пул потоков с разделяемой очередью задач.

Пул потоков подходит для вычислительно тяжелых и независимых задач. Но задачи файберов такими не являются: они короткие и часто зависят друг от друга (через файберные примитивы синхронизации).

### Быстрые задачи и contention на очереди

Задачи для файберов получаются очень короткими, так что накладные расходы на планирование этих задач становятся сопоставимы с их исполнением.

При этом процедура планирования задачи упирается в общую очередь, т.е. в общий мьютекс, который и ограничивает масштабируемость: число критических секций в единицу времени для мьютекса с ростом числа ядер не просто не растет, а даже уменьшается.

### Коммуникация, локальность, кэши

Кроме того, файберы могут интенсивно коммуницировать между собой через примитивы синхронизации (мьютексы и каналы), 
явно (в случае каналов) или неявно (в случае мьютексов) передавая друг другу сообщения.

Если такие зависимые задачи запускать на одном ядре и последовательно, то можно получить выигрыш за счет того, что получатель 
"сообщения" прочтет его прямо из кэша процессора, ему не потребуется идти за ним в оперативную память.

## Дизайн

Центральная идея – _шардирование_.

### Шардирование

У каждого потока-воркера есть собственная ограниченная очередь задач (будем называть эту очередь _локальной_), воркер работает в первую очередь с ней. 

Локальные очереди позволяют воркерам
- Избегать координации друг с другом, что повышает пропускную способность планировщика.
- Кластеризовать коммуницирующие задачи (например, файберы) на одном ядре, что повышает эффективность кэшей.

С другой стороны, шардирование усложняет: 
1) Балансировку нагрузки между воркерами, так как задачи могут распределиться и генерироваться на воркерах неравномерно. 
2) Парковку воркеров: больше нельзя атомарно проверить пустоту общей очереди и запарковать поток на кондваре.

### Балансировка нагрузки

Для балансировки нагрузки воркеры используют два механизма:

- разделяемую очередь задач и 
- воровство задач (_work-stealing_).

Воркеры разделяют общую очередь задач неограниченной емкости (будем называть ее _глобальной_): в нее они выгружают излишки задач из локальной очереди, из нее они забирают новые задачи, когда локальная очередь опустошается.

Когда и локальная, и глобальная очереди оказываются пусты, воркеры воруют задачи прямо из локальных очередей других воркеров ([Work stealing](https://en.wikipedia.org/wiki/Work_stealing)).

### LIFO

Для более эффективного исполнения коммуницирующих задач планировщик использует LIFO-планирование:

К локальной очереди на каждом воркере добавляется LIFO-слот, который имеет наивысший приоритет при выборе очередной задачи для исполнения.

### Алгоритм

#### Планирование задачи

- Если задача планируется из воркера пула, то она добавляется в хвост локальной очереди либо в LIFO-слот, если пользователь явно попросил о последнем с помощью специальной подсказки.

- Если задача планируется из внешнего (по отношению к пулу) потока, то она добавляется в хвост общей очереди.

#### Выбор задачи для запуска

Воркер перебирает следующие источники задач (в порядке убывания приоритета):

1) LIFO-слот
2) Локальная очередь
3) Глобальная очередь
4) Work stealing 

#### Semi-Fairness

Каждая запланированная в пул потоков задача должна быть исполнена.

Однако если задачу выбирать в соответствии с планом выше, то возможны сценарии, когда задача находится в планировщике, но воркеры ее не запускают:

1) Задача находится в глобальной очереди, при этом в локальных очередях воркеров всегда остаются задачи, так что воркеры не проверяют глобальную очередь.

2) Два файбера обмениваются сообщениями через каналы, по циклу запуская друг друга через LIFO-слот, так что воркер вообще не проверяет локальную очередь.

Для решения первой проблемы периодически опрашивайте глобальную очередь задач _до_ проверки LIFO-слота (вспомните фундаментальную константу Дмитрия Вьюкова – 61).

Для решения второй проблемы ограничивайте число последовательных запусков задач через LIFO-слот. Можно думать об этом так: задача, взятая из LIFO-слота, наследует квант планирования предшествующей задачи.

#### Парковка

Поток-воркер, который не смог найти задачи для исполнения, должен парковаться и не тратить процессорное время.

Воркер НЕ должен парковаться, если в очередях пула остаются невыполненные задачи.

## Реализация

### Интрузивность

Для реализации планировщика потребуются _интрузивные_ задачи.

Под интрузивной мы понимаем задачу, которая
- сама управляет временем своей жизни и
- умеет встраиваться во внутренние структуры данных планировщика.

Интрузивная задача реализует интерфейс [`ITask`](exe/executors/task.hpp) с единственным методом `Run` – выполнить задачу.

Интрузивная задача наследуется от `TaskBase`, в который дополнительно помещается указатель
для связывания задач в очереди.

### Локальная очередь

[`WorkStealingQueue`](exe/executors/tp/fast/queues/work_stealing_queue.hpp)

Локальная очередь задач представляет собой лок-фри циклический буфер фиксированного размера.

- Методы `TryPush` и `TryPop` вызывает только владеющий очередью воркер.
- Метод `Grab` вызывают
  - другие воркеры при воровстве задач,
  - владеющий очередью воркер для выгрузки задач в общую очередь при переполнении локальной.

### Глобальная очередь

[`GlobalQueue`](exe/executors/tp/fast/queues/global_queue.hpp)

Глобальная очередь
- принимает задачи от "внешних" `Submit`-ов,
- помогает балансировать задачи между воркерами.

#### Offload

Если локальная очередь воркера переполняется в результате планирования новой задачи, то
половина задач из нее выгружается в глобальную очередь с помощью метода `OffloadTasksToGlobalQueue`.

Минимизируйте работу в критической секции при выгрузке задач в глобальную очередь: используйте в `GlobalQueue` [интрузивный список](https://gitlab.com/Lipovsky/wheels/-/blob/master/wheels/intrusive/forward_list.hpp),
добавляйте в него пачку задач за O(1) с помощью метода `Append`.

#### Grab

Если локальная очередь воркера оказывается пуста, воркер пытается забрать пачку задач из глобальной очереди с помощью метода `TryGrabTasksFromGlobalQueue`.

Подумайте как выбрать размер пачки чтобы аккуратнее балансировать нагрузку между воркерами.

### Work stealing

Если локальная и глобальная очередь пусты, воркер ворует напрямую из локальных очередей других воркеров.

Ограничивайте число одновременно ворующих воркеров: если в пуле остается мало работы, то воркеры не должны праздно перехватывать задачи друг у друга. Изучите как устроено это ограничение в планировщиках Go и Tokio.

Рандомизируйте порядок обхода "жертв" с помощью [`mt19937_64`](https://en.cppreference.com/w/cpp/numeric/random).

Для инициализации вихря Мерсенна в воркере используйте [`random_device` из `twist`](https://gitlab.com/Lipovsky/twist/-/blob/master/twist/ed/stdlike/random.hpp): он обеспечит детерминизм исполнения при запуске тестов под файберами.

### LIFO-слот и честность

Поддержите LIFO-слот при планировании: задача, запланированная прямо из пула потоков, попадает в этот слот и запускается уже на следующей итерации воркера.

Если при планировании задачи в LIFO-слот оказывается, что тот уже занят другой задачей, то последняя вытесняется в хвост локальной очереди.

Ограничьте число последовательных итераций планирования через LIFO-слот для обеспечения честности.

#### Подсказки

Дайте пользователю (например, файберам) возможность управлять планированием: поддержите в методе `Submit` (и в `FiberHandle::Schedule`) дополнительный параметр [`SchedulerHint`](exe/executors/hint.hpp) – подсказку планировщику.

Подсказки недирективны: корректность кода, который дает подсказку планировщику, не должна зависеть от того, была ли эта подсказка учтена при планировании.

#### Default

По умолчанию `fast::ThreadPool` должен использовать FIFO-планирование. 

Пользователь, который ставит в приоритет производительность, должен переходить на LIFO-планирование явно, указывая `SchedulerHint::Next`.

### `Yield`

По умолчанию задачи, которые планируются из самого планировщика, обслуживаются текущим потоком-воркером: попадают либо в его локальную очередь, либо в LIFO-слот.

Для функции `Yield` такое поведение не подходит: файбер хочет уступить исполнение всем файберам, а не только файберам из локальной очереди.

Сделайте для `Yield` отдельную подсказку и отправляйте файбер в конец глобальной очереди.

### Координация / парковка воркеров

Воркеры стараются работать независимо, но иногда им необходима координация, например, для парковки.

Изучите [механизм парковки потоков](https://github.com/golang/go/blob/d2552037426fe5a190c74172562d897d921fe311/src/runtime/proc.go#L31) в планировщике `Go`.

Изучите [реализацию](https://github.com/tokio-rs/tokio/blob/master/tokio/src/runtime/scheduler/multi_thread/idle.rs) этого механизма в планировщике [Tokio](https://github.com/tokio-rs/tokio).

Используйте аналогичную двухфазную парковку, реализуйте ее через класс [`Coordinator`](exe/executors/tp/fast/coordinator.hpp). 

Проверка очередей на пустоту после анонса парковки – метод `TryPickTaskBeforePark`.

Непосредственно для парковки воркера используйте отдельный атомик `wakeups`.

### `WaitIdle`

🚨 У планировщика в этой задаче не будет метода `WaitIdle`!

Наивная реализация этого метода через подсчет задач добавляет точку контеншена на быстром пути `Submit`.

Вместо использования `WaitIdle` мы попросим пользователя пула реализовать ожидание задач своими силами с помощью [`exe::threads::WaitGroup`](exe/threads/wait_group.hpp).

[Бонусный уровень] Придумайте и реализуйте `WaitIdle`, который не будет требовать на быстром пути `Submit` инкрементов разделяемого атомика.

### `thread_local`

Используйте [`TWISTED_THREAD_LOCAL_PTR`](https://gitlab.com/Lipovsky/twist/-/blob/master/examples/local/main.cpp) чтобы не платить
за эмуляцию тред-локального указателя для файберов из `twist`.

### Аллокации

Планировщик не должен аллоцировать динамическую память при планировании / исполнении задач:
- Локальная очередь – фиксированный буфер указателей на задачи
- Глобальная очередь – интрузивный контейнер для задач

### Лок-фри

[Бонусный уровень]

Избавьтесь от взаимного исключения в реализации, добейтесь гарантии лок-фри.

Вы можете использовать решения задач [`lockfree`](/tasks/lockfree).

Вы можете использовать `moodycamel::ConcurrentQueue`:

- https://github.com/cameron314/concurrentqueue
- [A Fast General Purpose Lock-Free Queue for C++](https://moodycamel.com/blog/2014/a-fast-general-purpose-lock-free-queue-for-c++.htm)
- [Detailed Design of a Lock-Free Queue](https://moodycamel.com/blog/2014/detailed-design-of-a-lock-free-queue.htm)

Заголовок: `<concurrentqueue.h>`

Внимательно изучите свойства этой очереди перед ее применением!

### Метрики

Собирайте [метрики](exe/executors/tp/fast/metrics.hpp), например:

- Сколько задач было запущено через LIFO-слот / локальную очередь / глобальную очередь
- Сколько раз воркеры воровали задачи, выгружали задачи в глобальную очередь, забирали задачи из глобальной очереди 
- Сколько раз воркеры парковались из-за отсутствия задач
- и т.п.

Метрики помогут понять поведение вашей реализации в разных сценариях нагрузки.

Собирайте метрики для каждого воркера независимо, без синхронизации.

### Конфиг

Поддержите конфиг для свободных параметров, которые возникают в реализации: так будет удобнее исследовать влияние этих параметров на поведение пула.

## Профилирование

Профилируйте вашу реализацию на [`workloads`](workloads/main.cpp) чтобы понять, на что вы расходуете время при планировании.

[Flame Graphs](https://www.brendangregg.com/flamegraphs.html)

В релизной сброке код будет собираться с хорошим аллокатором: https://github.com/microsoft/mimalloc 

## Задание

### Подготовительные шаги

0) Перенесите `WaitGroup` из задачи [cond/wait_group](/tasks/cond/wait_group) в [`exe/threads`](exe/threads/wait_group.hpp)
1) Реализуйте интрузивные задачи в [tasks/executors](/tasks/tasks/executors), без них хороший планировщик написать не получится.
2) Перенесите пул потоков из [tasks/executors](/tasks/tasks/executors), он послужит baseline-ом для нового планировщика.
3) Перенесите файберы из [fibers/mutex](/tasks/fibers/mutex), адаптируйте их для запуска в произвольном экзекуторе.
4) Пройдите тесты, установив в качестве [дефолтного пула](exe/executors/thread_pool.hpp) пул [`tp::compute::ThreadPool`](exe/executors/tp/compute/thread_pool.hpp).

### Планировщик

4) Реализуйте шардированный планировщик с локальными очередями и общей очередью для балансировки нагрузки.
5) Реализуйте work stealing.
6) Поддержите подсказки для экзекуторов и LIFO-планирование через выделенный слот.
7) [Бонусный уровень] Реализуйте парковку потоков планировщика с помощью координатора (тесты – [pipelines/thread_pool/parking.json](pipelines/thread_pool/parking.json) и [pipelines/thread_pool/balancing.json](pipelines/thread_pool/balancing.json))
8) [Бонусный уровень] Поддержите `WaitIdle` без оверхеда на быстром пути `Submit` (тесты – [pipelines/thread_pool/wait_idle.json](pipelines/thread_pool/wait_idle.json)).
9) [Бонусный уровень] Исключите взаимное исключение из реализации, добейтесь гарантии лок-фри

## Тесты

Тесты проверяют не конкретную реализацию пула потоков, а `executors::ThreadPool`, определенный в [exe/executors/thread_pool.hpp](exe/executors/thread_pool.hpp).

Вы можете переключать этот alias между реализациями во время отладки.

## References

### Work Stealing

- [Scheduling Multithreaded Computations by Work Stealing](http://supertech.csail.mit.edu/papers/steal.pdf)
- https://en.wikipedia.org/wiki/Work_stealing

### Дизайн

- [Scalable Go Scheduler Design Doc](https://golang.org/s/go11sched)
- [Go scheduler: Implementing language with lightweight concurrency](https://www.youtube.com/watch?v=-K11rY57K7k)
- [Making the Tokio scheduler 10x faster](https://tokio.rs/blog/2019-10-scheduler)

### Реализации

- [Golang](https://github.com/golang/go/blob/master/src/runtime/proc.go)
- [Tokio (Rust)](https://github.com/tokio-rs/tokio/tree/master/tokio/src/runtime/scheduler/multi_thread)
- [Kotlin](https://github.com/Kotlin/kotlinx.coroutines/blob/master/kotlinx-coroutines-core/jvm/src/scheduling/CoroutineScheduler.kt)
- [Cats Effect (Scala)](https://github.com/typelevel/cats-effect/blob/series/3.x/core/jvm/src/main/scala/cats/effect/unsafe/WorkStealingThreadPool.scala)