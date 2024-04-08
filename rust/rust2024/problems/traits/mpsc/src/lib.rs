#![forbid(unsafe_code)]

use std::{cell::RefCell, collections::VecDeque, fmt::Debug, rc::Rc};
use thiserror::Error;

////////////////////////////////////////////////////////////////////////////////

struct Buff<T> {
    buff: VecDeque<T>,
    sender_count: usize,
    closed: bool,
}

impl<T> Buff<T> {
    fn new() -> Self {
        Self {
            buff: VecDeque::new(),
            sender_count: 1,
            closed: false,
        }
    }

    fn is_closed(&self) -> bool {
        self.sender_count == 0 || self.closed
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Error, Debug)]
#[error("channel is closed")]
pub struct SendError<T> {
    pub value: T,
}

pub struct Sender<T> {
    shared_state: Rc<RefCell<Buff<T>>>,
}

impl<T> Sender<T> {
    pub fn send(&self, value: T) -> Result<(), SendError<T>> {
        if self.is_closed() {
            Err(SendError { value })
        } else {
            self.shared_state.borrow_mut().buff.push_back(value);
            Ok(())
        }
    }

    pub fn is_closed(&self) -> bool {
        self.shared_state.borrow().is_closed()
    }

    pub fn same_channel(&self, other: &Self) -> bool {
        Rc::ptr_eq(&self.shared_state, &other.shared_state)
    }
}

impl<T> Clone for Sender<T> {
    fn clone(&self) -> Self {
        self.shared_state.borrow_mut().sender_count += 1;
        Sender {
            shared_state: Rc::clone(&self.shared_state),
        }
    }
}

impl<T> Drop for Sender<T> {
    fn drop(&mut self) {
        self.shared_state.borrow_mut().sender_count -= 1;
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Error, Debug)]
pub enum ReceiveError {
    #[error("channel is empty")]
    Empty,
    #[error("channel is closed")]
    Closed,
}

pub struct Receiver<T> {
    shared_state: Rc<RefCell<Buff<T>>>,
}

impl<T> Receiver<T> {
    pub fn recv(&mut self) -> Result<T, ReceiveError> {
        let closed = self.shared_state.borrow().is_closed();
        self.shared_state
            .borrow_mut()
            .buff
            .pop_front()
            .ok_or(if closed {
                ReceiveError::Closed
            } else {
                ReceiveError::Empty
            })
    }

    pub fn close(&mut self) {
        self.shared_state.borrow_mut().closed = true;
    }
}

impl<T> Drop for Receiver<T> {
    fn drop(&mut self) {
        self.close();
    }
}

////////////////////////////////////////////////////////////////////////////////

pub fn channel<T>() -> (Sender<T>, Receiver<T>) {
    let shared_state = Rc::new(RefCell::new(Buff::new()));
    (
        Sender {
            shared_state: Rc::clone(&shared_state),
        },
        Receiver {
            shared_state: Rc::clone(&shared_state),
        },
    )
}
