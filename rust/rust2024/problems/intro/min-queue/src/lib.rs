#![forbid(unsafe_code)]

use std::collections::VecDeque;

#[derive(Default)]
pub struct MinQueue<T> {
    data: VecDeque<T>,
    min_q: VecDeque<T>,
}

impl<T: Clone + Ord> MinQueue<T> {
    pub fn new() -> Self {
        MinQueue {
            data: VecDeque::new(),
            min_q: VecDeque::new(),
        }
    }

    pub fn push(&mut self, val: T) {
        self.data.push_back(val.clone());
        while self.min_q.back() > Some(&val) {
            self.min_q.pop_back();
        }
        if self.min_q.back() <= Some(&val) {
            self.min_q.push_back(val);
        }
    }

    pub fn pop(&mut self) -> Option<T> {
        match self.data.pop_front() {
            Some(el) => {
                if Some(&el) == self.min_q.front() {
                    self.min_q.pop_front();
                }
                Some(el)
            }
            None => None,
        }
    }

    pub fn front(&self) -> Option<&T> {
        self.data.front()
    }

    pub fn min(&self) -> Option<&T> {
        self.min_q.front()
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }
}
