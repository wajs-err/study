# Взаимная блокировка

- _The plural of "index" is "indices"_
- _The plural of "vertex" is "vertices"_
- _The plural of "mutex" is "deadlock"_

---

В этой задаче вам нужно воспроизвести самый известный многопоточный баг – *взаимную блокировку* или *дэдлок* (*deadlock*).

Назовем *взаимной блокировкой* такое состояние исполнения, в котором ни один из потоков не может продолжить работу независимо от дальнейшего поведения планировщика, поскольку каждый поток заблокировался на ожидании другого.

Дэдлок – терминальная конфигурация, выйти из него нельзя. В этом его отличие от *лайвлока* (*livelock*), который разрешится сам по себе при удачном планировании потоков.

## Потоки и недетерминизм

Воспроизводить многопоточные баги сложно из-за недетерминизма исполнения многопоточного кода, который обусловлен двумя обстоятельствами:
- Потоки исполняются физически параллельно на разных ядрах процессора
- Поток в любой момент может быть _вытеснен_ с ядра планировщиком операционной системы по прерыванию от таймера или IO.

В итоге одна и та же программа с дэдлоком от запуска к запуску может как попадать в этот дэдлок, так и обходить его.

## TinyFibers

Чтобы преодолеть недетерминизм, мы заменим потоки операционной системы *однопоточными кооперативными файберами* – [_TinyFibers_](https://gitlab.com/Lipovsky/tinyfibers):

```cpp
tf::rt::RunScheduler([] {
  for (size_t i = 0; i < 3; ++i) {
    tf::Yield();
  }
});
```

Планировщик _TinyFibers_ реализован в пространстве пользователя и планирует файберы в одном потоке операционной системы.

Файберы в _TinyFibers_ _кооперативные_, они отдают управление планировщику только добровольно и явно:
- При вызове `Yield()` или `SleepFor(delay)`
- При вызове `mutex.Lock()` на [мьютексе](https://en.cppreference.com/w/cpp/thread/mutex), которым в данный момент владеет другой файбер

Планировщик не может вытеснить бегущий файбер и заменить его другим по собственной воле.

## Детерминированная симуляция

Заменив параллельно исполняемые вытесняемые потоки операционной системы на
однопоточные кооперативные файберы, планируемые в пространстве пользователя, и вручную управляя их переключениями,
мы сможем строить детерминированные конкурентные исполнения.

В данной задаче мы воспользуемся этой техникой для моделирования взаимной блокировки,
а в последующих задачах курса мы будем (неявно) использовать файберы для [детерминированного тестирования решений](https://gitlab.com/Lipovsky/twist).

## Задание

Сначала посмотрите на [_TinyFibers_](https://gitlab.com/Lipovsky/tinyfibers):
- изучите [примеры](https://gitlab.com/Lipovsky/tinyfibers/-/blob/master/examples/),
- прочтите [документацию](https://gitlab.com/Lipovsky/tinyfibers/-/blob/master/docs/api.md),
- напишите собственный код в [`play/main.cpp`](https://gitlab.com/Lipovsky/tinyfibers/-/blob/master/play/main.cpp).

А затем заведите эти файберы в дэдлок:

Вам даны две [симуляции](sims), заполните в них лямбды файберов с комментарием
```cpp
// I am a Fiber
```

Вы можете использовать:
- `mutex.Lock()` / `mutex.Unlock()` для захвата / освобождения мьютексов,
- `Yield()` для переключения на следующий в очереди планировщика файбер.

Запускать новые файберы, заводить новые мьютексы или подключать новые заголовки не нужно.

## Блокировки и драконы

Если вам понравилось управлять планировщиком и ломать многопоточный код, то непременно поиграйте в [Deadlock Empire](https://deadlockempire.github.io/)!