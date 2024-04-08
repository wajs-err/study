#![forbid(unsafe_code)]

use std::{cell::RefCell, collections::VecDeque, mem::swap, rc::Rc};

pub struct LazyCycle<I>
where
    I: Iterator,
    I::Item: Clone,
{
    iter: I,
    values: Vec<I::Item>,
    curr: Option<usize>,
}

impl<I: Iterator> Iterator for LazyCycle<I>
where
    I::Item: Clone,
{
    type Item = I::Item;

    fn next(&mut self) -> Option<Self::Item> {
        if let Some(curr) = self.curr {
            self.curr = self.curr.map(|curr| (curr + 1) % self.values.len());
            Some(self.values[curr].clone())
        } else {
            match self.iter.next() {
                Some(val) => {
                    self.values.push(val.clone());
                    Some(val)
                }
                None => {
                    if self.values.is_empty() {
                        None
                    } else {
                        self.curr = Some(1);
                        Some(self.values[0].clone())
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

pub struct Extract<I: Iterator> {
    iter: I,
    values: VecDeque<I::Item>,
}

impl<I: Iterator> Iterator for Extract<I> {
    type Item = I::Item;

    fn next(&mut self) -> Option<Self::Item> {
        if !self.values.is_empty() {
            self.values.pop_front()
        } else {
            self.iter.next()
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

struct TeeState<I: Iterator> {
    iter: I,
    values: VecDeque<I::Item>,
    added: usize,
    len: usize,
    started: bool,
}

pub struct Tee<I>
where
    I: Iterator,
    I::Item: Clone,
{
    state: Rc<RefCell<TeeState<I>>>,
    curr: usize,
    done: bool,
}

impl<I: Iterator> Iterator for Tee<I>
where
    I::Item: Clone,
{
    type Item = I::Item;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }

        self.curr += 1;
        if self.curr <= self.state.borrow().added {
            if self.curr == self.state.borrow().len {
                self.done = true;
            }
            self.state.borrow_mut().values.pop_front()
        } else {
            if self.state.borrow().started && self.curr == 1 {
                self.done = true;
                return None;
            }

            let mut b = self.state.borrow_mut();
            b.started = true;
            match b.iter.next() {
                Some(value) => {
                    b.added += 1;
                    b.values.push_back(value.clone());
                    Some(value)
                }
                None => {
                    b.len = self.curr - 1;
                    self.done = true;
                    None
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

pub struct GroupBy<I, F, V>
where
    I: Iterator,
    F: FnMut(&I::Item) -> V,
    V: Eq,
{
    iter: I,
    f: F,
    next_val: Option<I::Item>,
    prev_fval: Option<V>,
    done: bool,
}

impl<I: Iterator, F, V> Iterator for GroupBy<I, F, V>
where
    F: FnMut(&I::Item) -> V,
    V: Eq,
{
    type Item = (V, Vec<I::Item>);

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }

        let mut res = Vec::new();
        if self.prev_fval.is_none() {
            if let Some(val) = self.iter.next() {
                self.prev_fval = Some((self.f)(&val));
                res.push(val);
            } else {
                return None;
            }
        } else {
            let mut val = None;
            swap(&mut self.next_val, &mut val);
            res.push(val.unwrap());
        }

        for val in self.iter.by_ref() {
            let mut fval = Some((self.f)(&val));
            if fval == self.prev_fval {
                res.push(val);
            } else {
                swap(&mut self.prev_fval, &mut fval);
                self.next_val = Some(val);
                return Some((fval.unwrap(), res));
            }
        }
        self.done = true;
        Some(((self.f)(&res[0]), res))
    }
}

////////////////////////////////////////////////////////////////////////////////

pub trait ExtendedIterator: Iterator {
    fn lazy_cycle(self) -> LazyCycle<Self>
    where
        Self: Sized,
        Self::Item: Clone,
    {
        LazyCycle {
            iter: self,
            values: Vec::new(),
            curr: None,
        }
    }

    fn extract(mut self, index: usize) -> (Option<Self::Item>, Extract<Self>)
    where
        Self: Sized,
    {
        if index == 0 {
            let first = self.next();
            (
                first,
                Extract {
                    iter: self,
                    values: VecDeque::new(),
                },
            )
        } else {
            let values = self.by_ref().take(index).collect::<VecDeque<_>>();
            let nth = self.next();
            (nth, Extract { iter: self, values })
        }
    }

    fn tee(self) -> (Tee<Self>, Tee<Self>)
    where
        Self: Sized,
        Self::Item: Clone,
    {
        let rc = Rc::new(RefCell::new(TeeState {
            iter: self,
            values: VecDeque::new(),
            added: 0,
            len: 0,
            started: false,
        }));
        (
            Tee {
                state: rc.clone(),
                curr: 0,
                done: false,
            },
            Tee {
                state: rc.clone(),
                curr: 0,
                done: false,
            },
        )
    }

    fn group_by<F, V>(self, func: F) -> GroupBy<Self, F, V>
    where
        Self: Sized,
        F: FnMut(&Self::Item) -> V,
        V: Eq,
    {
        GroupBy {
            iter: self,
            f: func,
            next_val: None,
            prev_fval: None,
            done: false,
        }
    }
}

impl<I: Iterator> ExtendedIterator for I {}
