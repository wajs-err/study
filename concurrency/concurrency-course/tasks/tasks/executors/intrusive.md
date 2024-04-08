# Интрузивность

🛑 Приступайте к этой части **только если** вы можете написать собственный `std::function<void()>`.

---

Пока планирование задачи в экзекутор – это (в общем случае) динамическая аллокация контейнера для лямбды (назовем его _состоянием задачи_) для type erasure внутри `fu2::unique_function`.

![Non-intrusive tasks](https://gitlab.com/Lipovsky/concurrency-course-media/-/raw/main/tasks/executors/non-intrusive-tasks.png)

При этом главным "пользователям" экзекуторов – stackful файберам, stackless корутинам, фьючам – динамические аллокации при планировании не требуются: они могут управлять состоянием своих задач эффективнее. 

## Интрузивные задачи

Избавьтесь от динамических аллокаций памяти в экзекуторах, сделав задачи _интрузивными_:

```cpp
// exe/executors/task.hpp

struct ITask {
  virtual ~ITask() = default;
  
  virtual void Run() noexcept = 0;
};
```

За этим интерфейсом может находиться
- непосредственно `Fiber`
- awaiter для stackless корутины
- служебный контейнер для лямбды пользователя для свободной функции `Submit`

Задачам нужно будет укладываться в очереди, так что встроим в них указатель:

```cpp
// exe/executors/task.hpp

struct TaskBase : ITask, wheels::IntrusiveForwardListNode<TaskBase> {
  //
};
```

Метод `Submit` у `IExecutor` будет принимать `TaskBase*`:

```cpp
// exe/executors/executor.hpp

struct IExecutor {
  virtual ~IExecutor() = default;
  
  virtual void Submit(TaskBase* task) = 0;
};
```
Итого, в памяти выстроится следующая конструкция:

![Intrusive tasks](https://gitlab.com/Lipovsky/concurrency-course-media/-/raw/main/tasks/executors/intrusive-tasks.png)


- Файберы, корутины и фьючи будут использовать метод `Submit` у `IExecutor` напрямую, избегая ненужных аллокаций,
- Свободная функция `Submit` будет аллоцировать для лямбд служебную задачу-контейнер, которая будет самоуничтожаться при завершении.

## Задание

- Реализуйте интрузивные экзекуторы
- Перенесите их в задачу [fibers/mutex](/tasks/fibers/mutex) и избавьтесь в файберах от динамических аллокаций памяти при планировании
