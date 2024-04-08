# (Lazy) Future

## Пререквизиты

- [tasks/executors](/tasks/tasks/executors) + интрузивные задачи
- [futures/fun](/tasks/futures/fun)
- [tasks/scheduler](/tasks/tasks/scheduler) – рекомендуется

---

В этой задаче мы напишем _ленивую_ (_lazy_) реализацию функциональных фьюч.

Ленивая реализация позволит избавиться от избыточной синхронизации и динамических аллокаций памяти, которые возникают в _энергичной_ (_eager_, _бодрой_, _жадной_) реализации.

## Пример

```cpp
// Здесь f – thunk, представляющий запуск лямбды в пуле потоков
futures::Future<int> auto f = futures::Submit(pool, [] {
  return result::Ok(42);
});

// <- Пока задача не была отправлена в пул потоков

// Терминатор `Get` форсирует вычисление thunk-а / задача с лямбдой пользователя посылается в пул потоков
auto r = std::move(f) | futures::Get();
```

## References

Сначала – полезная для понимания дизайна теория:

#### Haskell

- [A History of Haskell: being lazy with class](https://www.youtube.com/watch?v=06x8Wf2r2Mc) by SPJ

##### Laziness

- [Lazy evaluation](https://wiki.haskell.org/Lazy_evaluation), [Thunk](https://wiki.haskell.org/Thunk)
- [The Incomplete Guide to Lazy Evaluation (in Haskell)](https://apfelmus.nfshost.com/articles/lazy-eval.html)
- [Thunks, Sharing, Laziness: The Haskell Heap Visualized](https://www.youtube.com/watch?v=I4lnCG18TaY)

##### Semantics

- [Non-strict semantics](https://wiki.haskell.org/Non-strict_semantics)
- [Non-Strict Semantics of Haskell](https://apfelmus.nfshost.com/articles/non-strict-semantics.html)

#### λ-calculus
- [Introduction to λ-calculus](https://www.cs.cornell.edu/courses/cs3110/2011sp/Lectures/lec27-lambda/lambda.htm)
- [Church–Rosser theorem](https://en.wikipedia.org/wiki/Church%E2%80%93Rosser_theorem)

## Ленивая модель

Фьюча – это _thunk_ (далее – просто _санк_), т.е. объект, который представляет еще не стартовавшую асинхронную операцию.

Санк форсируется / асинхронная операция стартует только когда ее результат понадобился некоторому _потребителю_ (_consumer_).

Потребитель – это 
- поток, файбер или корутина, которые хотят "развернуть" фьючу и получить результат,
- либо комбинатор, который в свою очередь форсирован потоком, файбером или корутиной.

Вычисляясь, санк отправляет потребителю _output_ = _result_ + _context_.

Здесь _context_ – это мутабельное состояние фьючи (например, экзекьютор).

### `futures/model`

Все понятия модели явно представлены в коде:

- [`Context`](exe/futures/model/context.hpp)
- [`Output<T>`](exe/futures/model/output.hpp)
- [`IConsumer<T>`](exe/futures/model/consumer.hpp)
- [`Thunk`](exe/futures/model/thunk.hpp)

### Фьючи

Фьюча – не конкретный класс, а [концепт](https://en.cppreference.com/w/cpp/language/constraints) [`SomeFuture`](exe/futures/types/naked.hpp) (он же [`Thunk`](exe/futures/model/thunk.hpp)) + типизированный `Future<T>`.

Планируя цепочку шагов с помощью API фьюч, пользователь конструирует в compile time конкретный санк:

```cpp
// Здесь f – тип, в котором закодированы все шаги цепочки
futures::Future<int> auto f = futures::Just()
    | futures::Via(pool)
    | futures::AndThen([](Unit) {
         // Some computation goes here...
         return result::Ok(1);
      })
    | futures::Map([](int v) {
        return v + 1;
      });  
```

### Терминаторы

В ленивой реализации раскрывается линейность фьюч / требование завершать любую цепочку терминатором:

Терминатор – это шаг, на котором форсируется вычисление санка / запускается асинхронная операция.

Поэтому в ленивой реализации мы переименуем `exe/futures/terminate` в `exe/futures/run`.

### Storage

**Интрузивность** в экзекуторах дает пользователю (в данном контексте – фьючам) возможность управлять стораджем состояния задачи (например, в `Submit` – стораджем лямбды с кодом пользователя).

**Ленивость** позволяет отложить выбор стораджа (стек или куча) для состояния задачи / операции до
того момента, когда можно будет сделать оптимальный выбор, т.е. до применения терминатора!

Варианты:

- Для синхронного терминатора (`Get` для потоков / `Await` для файберов / `co_await` для корутин) достаточно
положить санк на его стековый фрейм. Таким образом, из исполнения стираются динамические аллокации.

- Для `Detach` придется упаковать санк в самоутилизирующийся "контейнер" на куче: пути текущего исполнения и асинхронного расходятся.

### Rendezvous

Запуск асинхронной операции упорядочивается с подпиской на результат этой операции, 
а значит рандеву между продьюсером и консьюмером в ленивой реализации не нужно.

Синхронизация в реализации фьюч остается только там, где есть недетерминизм на уровне логики пользователя, 
т.е. в параллельных комбинаторах.

### Profit

Исполнение цепочки из следующего сниппета не требует
- синхронизации (за исключением синхронизации в самом планировщике, а в хорошем планировщике на быстром пути она минимальна),
- динамических аллокаций памяти.

```cpp
auto f = futures::Submit(pool, [] { return result::Ok(7) }) 
    | futures::FlatMap([](int v) {
        return futures::Submit(pool, [v] {
          return result::Ok(v + 1);
        })
      })
    | futures::OrElse([](Error) {
        return result::Ok(42);
      });

auto r = std::move(f) | futures::Get();
```

## Комбинаторы

Ленивым фьючам потребуются новые комбинаторы:

---

### 👉  `Box`

Заголовок: [`combine/seq/box.hpp`](exe/futures/combine/seq/box.hpp)

Стирает конкретный тип санка до `BoxedFuture<T>` / переносит санк на кучу.

```cpp
auto f = futures::Submit(pool, [] {
  return result::Ok(42);
}) | futures::Box();
```

Или

```cpp
// Auto-boxing
futures::BoxedFuture<int> f = futures::Value(7);
```

#### Интерфейсы

Боксинг необходим асинхронным интерфейсам:

```cpp
struct IAsyncReader {
  virtual ~IAsyncReader() = default;
  
  // Не можем использовать Future<size_t>, должны указать конкретный тип
  virtual exe::futures::BoxedFuture<size_t> ReadSome(wheels::MutableMemView buffer) = 0;
};
```

См. также [Stabilizing async fn in traits in 2023](https://blog.rust-lang.org/inside-rust/2023/05/03/stabilizing-async-fn-in-trait.html)

---

### 👉  `Start`

Заголовок: [`combine/seq/start.hpp`](exe/futures/combine/seq/start.hpp)

Форсирует вычисление санка / трансформирует ленивую фьючу в энергичную / стартует асинхронную операцию.

```cpp
futures::EagerFuture<int> f = futures::Submit(pool, [] {
  return result::Ok(7);
}) | futures::Start();  // <- Отправляем задачу в пул потоков

// <- Задача уже отправлена в пул потоков
```

Синоним: `futures::Force()`

#### Bang (!)

Заголовок: [`syntax/bang.hpp`](exe/futures/syntax/bang.hpp)

По аналогии с [bang patterns в Haskell](https://ghc.gitlab.haskell.org/ghc/doc/users_guide/exts/strict.html), поддержим для фьюч [оператор `!` (_bang_)](exe/futures/syntax/bang.hpp):

```cpp
// !f читается как "bang f", означает f | Force()
auto f = !futures::Submit(pool, [] {
  return result::Ok(7);
});
```

---

## Senders

Бонусный уровень!

Найдите в реализации статически конструируемые односвязные списки, переверните их в compile-time и избавьтесь таким образом от динамической диспетчеризации вызовов.

В модели ленивых фьюч при этом возникнет новое понятие.

Вы получите реализацию, эквивалентную 🔥 [`std::execution`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2300r6.html)

## Указания по реализации

- Санки для фьюч поместите в `futures/thunks`
- Маркируйте санки [`[[nodiscard]]`](https://en.cppreference.com/w/cpp/language/attributes/nodiscard)
- Запретите санкам копироваться
- При реализации `IConsumer` помечайте санки как `final`
