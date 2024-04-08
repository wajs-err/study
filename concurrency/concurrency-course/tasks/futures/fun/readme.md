# (Functional) Future

## Пререквизиты

- [cond/wait_group](/tasks/cond/wait_group)
- [futures/std](/tasks/futures/std)
- [tasks/executors](/tasks/tasks/executors)

---

До этого момента мы писали concurrency в императивном стиле, думая про конкурентные активности как про серии мутаций разделяемого состояния и представляя их файберами. 

В этой задаче предлагается [функциональная](https://www.haskell.org/tutorial/) модель для concurrency на основе фьюч и комбинаторов, в которой конкурентные активности
представлены как графы трансформаций иммутабельных значений.

## `Future`

В основе модели лежит понятие _future_ (далее - просто _фьюча_) и класс `Future<T>`.

`Future<T>` представляет будущий, еще не готовый, результат асинхронной операции.

За фьючей может стоять
- [вычисление в пуле потоков](exe/futures/make/submit.hpp),
- [RPC](https://capnproto.org/cxxrpc.html),  
- [IO](https://tokio.rs/tokio/tutorial/io),
- [ожидание на семафоре](https://github.com/twitter/util/blob/143adbdf5bc6dc55eddb9248c597eeb11a799094/util-core/src/main/scala/com/twitter/concurrent/AsyncSemaphore.scala#L141),
- [таймаут](https://gitlab.com/Lipovsky/await/-/blob/c506831d7cce2099ceff5a433e100f460fabf542/examples/futures/main.cpp#L456) и т.д.

## `Result`

Фьюча может представлять вызов метода на удаленном сервере или чтение с диска,
и нужно учитывать, что подобная операция может завершиться ошибкой.

Поэтому непосредственно результат асинхронной операции в нашей модели будет представлен контейнером `Result<T>`, который содержит либо значение типа `T`, либо ошибку.

## Комбинаторы

Фьючи, т.е. стоящие за ними асинхронные операции, мы будем комбинировать в функциональном стиле:

```cpp
futures::Future<Response> Hedge(Request request) {
  // primary – фьюча, представляющая удаленный вызов
  auto primary = Rpc(request);

  auto backup = futures::After(99ms) | futures::FlatMap([request](Unit) {
    return Rpc(request);
  });
  
  return futures::First(std::move(primary), std::move(backup));
}
``` 

На этом небольшом примере с хэджированием запросов (см. [Tail at Scale](https://www.barroso.org/publications/TheTailAtScale.pdf)) хорошо видны преимущества функционального подхода в приложении к concurrency:

Разработчик описывает _что_ и _когда_ он хочет сделать, а за реализацию этого плана (и за всю сопряженную с ним синхронизацию) отвечают комбинаторы.

Меняется ментальная модель concurrency: разработчик думает не про чередование обращений к разделяемому состоянию, т.е. не про control flow, а про трансформацию иммутабельных значений, которые "текут" от продьюсеров к консьюмерам, т.е. про data flow.

См. 🔥 [Your Server as a Function](https://monkey.org/~marius/funsrv.pdf).

## Модель

Фьючи – значения, которые представляют цепочки задач или, в общем случае, произвольные конкурентные активности.

Операции:

- _конструкторы_ (`Contract`, `Submit`, `Value`, ...) строят фьючи / открывают цепочки,
- _комбинаторы_ (`Map`, `AndThen`, `OrElse`, ...) получают фьючи на вход, поглощают их и строят новые фьючи / продолжают цепочки,
- _терминаторы_ (`Get`, `Detach`) поглощают фьючи / завершают цепочки.

_Конструкторы_ моделируют продьюсеров, _терминаторы_ – консьюмеров.

Фьючи – _линейные_:
- фьюча не может быть скопирована,
- каждая фьюча должна быть поглощена ровно один раз комбинатором или терминатором.

## Пример

```cpp
void FuturesExample() {
  using namespace exe;
  
  executors::ThreadPool pool{4};
  pool.Start();
  
  Result<int> r = futures::Value(1)  // <- конструктор, начинает цепочку
      | futures::Via(pool) 
      | futures::AndThen([](int v) {  // <- комбинатор
          fmt::println("{}", v);
          return result::Ok(v + 1);
        }) 
      | futures::AndThen([](int v) -> Result<int> {
          return v * 2;
        })
      | futures::OrElse([](Error) {
          return result::Ok(7);  // fallback
        })  
      | futures::Get();  // <- терминатор, завершает цепочку
  
  fmt::println("{}", *r);  // Печатает 4
  
  pool.Stop();
}
```

Больше примеров – в [play/main.cpp](play/main.cpp)


## Структура `exe/futures`

- `types` – типы
- `make` – конструкторы
- `combine` – комбинаторы
  - `seq` – последовательная композиция
  - `par` – параллельная композиция
- `terminate` – терминаторы
- `syntax` – перегрузки операторов

## Конструкторы

Конструкторы – функции, конструирующие новые фьючи.

Каталог базовых конструкторов: [`futures/make`](exe/futures/make)

Фреймворк RPC расширит этот базовый набор конструктором удаленного вызова, библиотека IO – операциями асинхронного чтения / записи и т.д.

---

### 👉 `Contract`

Заголовок: [`make/contract.hpp`](exe/futures/make/contract.hpp)

Асинхронный контракт между продьюсером и консьюмером:

```cpp
auto [f, p] = futures::Contract<int>();

// Producer
std::move(p).SetValue(7);

// Consumer
auto r = std::move(f) | futures::Get();
```

Обязанности сторон:

- Продьюсер должен выполнить фьючу с помощью `Set` / `SetValue` / `SetError`
- Консьюмер должен потребить фьючу с помощью комбинатора или терминатора

---

### 👉 `Submit`

Заголовок: [`make/submit.hpp`](exe/futures/make/submit.hpp)

Фьюча, представляющая вычисление в пуле потоков.

Лямбда, переданная в `Submit`, должна возвращать `Result<T>`

```cpp
auto f = futures::Submit(pool, [] {
  return result::Ok(7);  // Упаковываем значение в Result
);
```

---

### 👉 `Value`

Заголовок: [`make/value.hpp`](exe/futures/make/value.hpp)

Фьюча, представляющая готовое значение.

```cpp
auto f = futures::Value(7);
```

---

### 👉 `Failure`

Заголовок: [`make/failure.hpp`](exe/futures/make/fail.hpp)

Фьюча, представляющая готовую ошибку.

```cpp
auto f = futures::Failure<int>(TimeoutError());
```

---

### 👉 `Just`

Заголовок: [`make/just.hpp`](exe/futures/make/just.hpp)

Фьюча, представляющая готовое значение типа [`Unit`](exe/result/types/unit.hpp).

```cpp
auto f = futures::Just()
    | futures::Via(pool) 
    | futures::AndThen([](Unit) {
        return result::Ok(42);
      });
```

Асинхронные операции, не возвращающие значения, моделируются с помощью `Future<Unit>`:

Фьючи – это будущие значения, но значений типа `void` не бывает, значит не должно быть и будущих `void`-ов.

## Комбинаторы

Комбинаторы – функции, которые получают фьючи на вход (потребляют их) и строят новые фьючи.

### Последовательная композиция

Каталог: [`futures/combine/seq`](exe/futures/combine/seq)

#### Pipeline operator

Последовательная композиция фьюч выполняется через оператор `|` (_pipeline operator_). 

Оператор не имеет прямого отношения к фьючам, он решает более общую задачу: упростить описание цепочек вызовов функций, где последующий вызов получает на вход выход предыдущего. 

Выражение `f(a) | h(x, y)` переписывается оператором `|` в `h(f(a), x, y)`

Пример применения в С++ – [`ranges`](https://en.cppreference.com/w/cpp/ranges)

Поддержка `|` для фьюч: [futures/syntax/pipe.hpp](exe/futures/syntax/pipe.hpp)

##### References

С помощью перегрузки оператора `|` мы эмулируем оператор `|>`:

- [OCaml] [Pipelining](https://cs3110.github.io/textbook/chapters/hop/pipelining.html)
- [JavaScript] [Pipe Operator (|>) for JavaScript](https://github.com/tc39/proposal-pipeline-operator/)
- [C++] [Exploring the Design Space for a Pipeline Operator](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2672r0.html)

---

#### 👉 `Map` 

Заголовок: [`combine/seq/map.hpp`](exe/futures/combine/seq/map.hpp)

Сигнатура: `Future<T>` → (`T` → `U`) → `Future<U>` ([нотация](https://www.haskell.org/tutorial/functions.html) заимствована из функциональных языков)

Комбинатор `Map`:
- применяет функцию пользователя (_мэппер_) к значению входной фьючи, если та выполняется успешно, или
- пробрасывает ошибку, не вызывая функцию пользователя, если входная фьюча выполняется с ошибкой.

```cpp
auto f = futures::Value(1) | futures::Map([](int v) {
  return v + 1;
});
```

Комбинатор `Map` (как и любой другой комбинатор) применяется к фьюче без ожидания будущего результата, он лишь планирует вызов продолжения через подписку на результат входной фьючи и строит новую (выходную) фьючу.

---

#### 👉 `AndThen` / `OrElse`

Заголовки:
- [`combine/seq/and_then.hpp`](exe/futures/combine/seq/and_then.hpp)
- [`combine/seq/or_else.hpp`](exe/futures/combine/seq/or_else.hpp)  

Сигнатура:
- `AndThen`: `Future<T>`  → (`T` → `Result<U>`) → `Future<U>`
- `OrElse`: `Future<T>` → (`Error` → `Result<T>`) → `Future<T>`

Комбинаторы `AndThen` / `OrElse` разделяют успешный путь / fallback в цепочке: 
- `AndThen` вызывается только на значениях, 
- `OrElse` – только на ошибках.

```cpp
auto f = futures::Failure<int>(TimeoutError()) | 
    | futures::AndThen([](int v) {
        return result::Ok(v + 1);
      })
    | futures::OrElse([](Error) {
        return result::Ok(7);  // Fallback
      });
```

Нетрудно заметить, что `AndThen` / `OrElse` – это асинхронный аналог для блоков `try` / `catch`.

---

#### 👉 `Via`

##### Гонка

Рассмотрим пример:

```cpp
// Гонка между продьюсером и консьюмером
auto f = futures::Submit(pool, [] { return result::Ok(7); })
    | futures::Map([](int v) { return v + 1; });
```

Поток, в котором запустится мэппер, зависит от того, как упорядочатся при рандеву
- выполнение фьючи в пуле потоков и 
- подписка на результат в комбинаторе `Map`.

Пользователь, планирующий с помощью фьюч цепочку асинхронных действий, 
должен иметь контроль на тем, в каком потоке (а вернее, экзекуторе) выполнится каждый ее шаг.

Такой контроль дает комбинатор `Via`: 

```cpp
auto f = futures::Submit(pool, [] { return result::Ok(7); })
    | futures::Via(pool)  // <- Устанавливаем экзекутор для продолжения
    | futures::AndThen([](int value) {
        // Гарантированно в пуле потоков `pool`
        fmt::println("{}", value);
        return result::Ok()
      });
```

Установленный через `Via` экзекутор **наследуется** по цепочке (пока не встретит параллельный комбинатор или новый `Via`).

##### State

Можно заметить, что `Via` – это мутация состояния, которая "просочилась" в функциональную модель.

---

#### 👉 `Flatten`

Заголовок: [`combine/seq/flatten.hpp`](exe/futures/combine/seq/flatten.hpp)

Сигнатура: `Future<Future<T>>` → `Future<T>`

"Уплощает" вложенную асинхронность:

```cpp
futures::Future<int> f = futures::Submit(pool, [&pool]() -> Result<Future<int>> {
  return futures::Submit(pool, [] {
    return result::Ok(7);
  })
}) | futures::Flatten();
```

---

#### 👉 `FlatMap`

Заголовок: [`combine/seq/flat_map.hpp`](exe/futures/combine/seq/flat_map.hpp)

Сигнатура: `Future<T>` → (`T` → `Future<U>`) → `Future<U>`

Комбинация `Map` + `Flatten`. Планирует асинхронное продолжение цепочки задач.

```cpp
futures::Future<int> f = futures::Submit(pool, [] {
  return Ok(1);
}) | futures::FlatMap([&pool](int v) {
  return futures::Submit(pool, [v] {
    return result::Ok(v + 1);
  });
});   
```

##### Монады

Фьючи с конструктором `Value` и комбинатором `FlatMap` образуют [монаду](https://wiki.haskell.org/All_About_Monads#The_Monad_class).

### Параллельная композиция

Каталог: [`combine/par`](exe/futures/combine/par)

---

#### 👉 `First`

Заголовок: [`combine/par/first.hpp`](exe/futures/combine/par/first.hpp)

Фьюча, представляющая первое значение / последнюю ошибку двух фьюч.

```cpp
auto timeout = futures::After(1s) 
    | futures::Map([](Unit) -> Result<int> {
        return result::Err(TimeoutError());
      });

auto compute = futures::Submit(pool, [] {
  return result::Ok(7);
})

// Вычисление с таймаутом, можно посахарить комбинатором `WithTimeout`
auto first = futures::First(std::move(compute), std::move(timeout));
```

---

#### 👉 `Both` / `All`

Заголовок: [`combine/par/all.hpp`](exe/futures/combine/par/all.hpp)

Фьюча, представляющая оба (все) значения / первую ошибку фьюч, поданных на вход:

```cpp
auto f = futures::Submit(pool, [] { return result::Ok(1); });
auto g = futures::Submit(pool, [] { return result::Ok(2); });

auto both = futures::Both(std::move(f), std::move(g));

// Синхронно ожидаем двух результатов
auto r = std::move(both) | futures::Get();
// Распаковываем значение (`std::tuple`)
auto [x, y] = r.value();
```

Синхронная распаковка фьючи `both` не эквивалентна последовательной синхронной распаковке
двух фьюч `f` и `g`: если вторая фьюча завершилась ошибкой, то ожидание первой будет прервано.

---

#### Экзекуторы

Параллельные комбинаторы **сбрасывают** экзекутор до `Inline`, так что пользователь должен явно установить его после объединения цепочек.

## Терминаторы

_Терминаторы_ завершают асинхронные цепочки / графы.

Каталог: [`futures/terminate`](exe/futures/terminate)

---

### 👉 `Get`

Заголовок: [`terminate/get.hpp`](exe/futures/terminate/get.hpp)

Терминатор `Get` блокирует текущий поток до готовности результата:

```cpp
// Планируем задачу в пул потоков и дожидаемся результата
Result<int> r = futures::Submit(pool, [] {
  return result::Ok(7);
}) | futures::Get();
```

Иначе говоря, `Get` синхронно "распаковывает" `Future` в `Result`.

---

### 👉 `Detach`

Заголовок: [`terminate/detach.hpp`](exe/futures/terminate/detach.hpp)

`Future` аннотирована `[[nodiscard]]` и должна быть явно поглощена консьюмером. 

Терминатор `Detach` поглощает фьючу и игнорирует ее результат.

```cpp
// Завершение задачи в пуле нас не интересует
futures::Submit(pool, [] {
  /* ... */
}) | futures::Detach();
```

### Операторы

В [`futures/syntax`](exe/futures/syntax) собраны перегрузки операторов для фьюч:

- `f | c` означает `c.Pipe(f)`
- `f or g` означает `First(f, g)`
- `f + g` означает `Both(f, g)`

#### Пример

```cpp
// Синхронно дожидаемся первого результата
auto r = (std::move(compute) or std::move(timeout)) | futures::Get();
```

## `Result`

`Result<T>` – это alias для `tl::expected<T, E>`, где `E` = `std::error_code`.

- https://github.com/TartanLlama/expected.
- [Документация](https://tl.tartanllama.xyz/en/latest/api/expected.html)

Мы фиксируем тип ошибки для простоты, в хорошем фреймворке у пользователя должна быть свобода в выборе типа `E`.

### Конструкторы

#### `Ok`

Позволяет не указывать в сигнатуре лямбды возвращаемый тип:

```cpp
auto f = futures::Value(7) | futures::AndThen([](int v) {
  return result::Ok(v + 1);
);
```

Альтернативный вариант:

```cpp
auto f = futures::Value(7) | futures::AndThen([](int v) -> Result<int> {
  return v + 1;   // Автоматическая упаковка
);
```

#### `Err`

```cpp
// Status – это alias для Result<Unit>
auto f = futures::Just() | futures::AndThen([](Unit) -> Status {
  return result::Err(TimeoutError());  // Тип значения выводится автоматически
});
```

## Задание

1) Реализуйте `futures::Contract` + `futures::Get` с помощью wait-free рандеву и подписки с коллбэком
2) [Бонусный уровень] Прочтите статью [Your Server as a Function](https://monkey.org/~marius/funsrv.pdf)
3) Реализуйте функциональные фьючи
   1) Конструкторы
   2) Последовательные комбинаторы
   3) Параллельные комбинаторы
4) [Бонусный уровень] [Познакомьтесь с языком Haskell](https://www.haskell.org/tutorial/), мы к нему еще вернемся
5) [Бонусный уровень] Поддержите лямбды, получающие на вход / возвращающие `void`

## Реализация

### `Future` – значение

Пользователь не вызывает методов у класса `Future`, он работает с фьючами как со значениями, т.е. передает (точнее, перемещает)
в комбинаторы и терминаторы.

Реализация при этом может вызывать служебные методы `Future`.

### Коллбэки

За исключением `Get`, в фьючах нигде нет блокирующего ожидания!

Внутренняя механика всех комбинаторов и терминаторов – подписка на разультат с помощью коллбэка.

Например, 
- в `Map` коллбэк планирует запуск мэппера в экзекутор,
- в `Get` коллбэк устанавливает результат и будит ждущий поток,
- в `First` – "гоняется" с другим коллбэком за право выполнить выходную фьючу.

### Синхронизация

Фьючам не требуется взаимное исключение, все терминаторы и комбинаторы в
задаче могут быть реализованы с гарантией прогресса wait-free.

## References

### Futures

- [Twitter Futures](https://twitter.github.io/finagle/guide/Futures.html), [Your Server as a Function](https://monkey.org/~marius/funsrv.pdf)
- [Futures for C++11 at Facebook](https://engineering.fb.com/2015/06/19/developer-tools/futures-for-c-11-at-facebook/), [Folly Futures](https://github.com/facebook/folly/blob/main/folly/docs/Futures.md)
- [Асинхронность в программировании](https://habr.com/ru/companies/jugru/articles/446562/)

### Errors

- [The Error Model](https://joeduffyblog.com/2016/02/07/the-error-model/)

### Haskell

- [A Gentle Introduction to Haskell](https://www.haskell.org/tutorial/)

### Monads

- [All About Monads](https://wiki.haskell.org/All_About_Monads)
- [Monads for functional programming](https://homepages.inf.ed.ac.uk/wadler/papers/marktoberdorf/baastad.pdf)
- [Functors, Applicatives, And Monads In Pictures](https://www.adit.io/posts/2013-04-17-functors,_applicatives,_and_monads_in_pictures.html)
