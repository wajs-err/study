#![forbid(unsafe_code)]

use std::{borrow::Borrow, iter::FromIterator, ops::Index};

////////////////////////////////////////////////////////////////////////////////

#[derive(Default, Debug, PartialEq, Eq)]
pub struct FlatMap<K, V>(Vec<(K, V)>);

impl<K: Ord, V> FlatMap<K, V> {
    pub fn new() -> Self {
        FlatMap(Vec::new())
    }

    pub fn len(&self) -> usize {
        self.0.len()
    }

    pub fn is_empty(&self) -> bool {
        self.0.is_empty()
    }

    pub fn capacity(&self) -> usize {
        self.0.capacity()
    }

    pub fn as_slice(&self) -> &[(K, V)] {
        self.0.as_slice()
    }

    pub fn insert(&mut self, key: K, value: V) -> Option<V> {
        match self.0.binary_search_by(|kv| kv.0.cmp(&key)) {
            Ok(index) => {
                self.0.push((key, value));
                Some(self.0.swap_remove(index).1)
            }
            Err(index) => {
                self.0.insert(index, (key, value));
                None
            }
        }
    }

    pub fn get<Q: ?Sized>(&self, key: &Q) -> Option<&V>
    where
        K: Borrow<Q>,
        Q: Ord,
    {
        match self.0.binary_search_by(|kv| kv.0.borrow().cmp(key)) {
            Ok(index) => Some(&self.0[index].1),
            Err(_) => None,
        }
    }

    pub fn remove<Q: ?Sized>(&mut self, key: &Q) -> Option<V>
    where
        K: Borrow<Q>,
        Q: Ord,
    {
        self.remove_entry(key).map(|kv| kv.1)
    }

    pub fn remove_entry<Q: ?Sized>(&mut self, key: &Q) -> Option<(K, V)>
    where
        K: Borrow<Q>,
        Q: Ord,
    {
        match self.0.binary_search_by(|kv| kv.0.borrow().cmp(key)) {
            Ok(index) => Some(self.0.remove(index)),
            Err(_) => None,
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

impl<Q: ?Sized + Ord, K: Ord + Borrow<Q>, V> Index<&Q> for FlatMap<K, V> {
    type Output = V;

    fn index(&self, key: &Q) -> &Self::Output {
        let index = self
            .0
            .binary_search_by(|kv| kv.0.borrow().cmp(key))
            .unwrap();
        &self.0[index].1
    }
}

impl<K: Ord, V> Extend<(K, V)> for FlatMap<K, V> {
    fn extend<T: IntoIterator<Item = (K, V)>>(&mut self, iter: T) {
        for elem in iter {
            self.insert(elem.0, elem.1);
        }
    }
}

impl<K: Ord, V> From<Vec<(K, V)>> for FlatMap<K, V> {
    fn from(value: Vec<(K, V)>) -> Self {
        let mut this = Self::new();
        for (k, v) in value.into_iter() {
            this.insert(k, v);
        }
        this
    }
}

impl<K: Ord, V> From<FlatMap<K, V>> for Vec<(K, V)> {
    fn from(value: FlatMap<K, V>) -> Self {
        value.0
    }
}

impl<K: Ord, V> FromIterator<(K, V)> for FlatMap<K, V> {
    fn from_iter<T: IntoIterator<Item = (K, V)>>(iter: T) -> Self {
        let mut this = Self::new();
        this.extend(iter);
        this
    }
}

impl<K: Ord, V> IntoIterator for FlatMap<K, V> {
    type Item = (K, V);
    type IntoIter = std::vec::IntoIter<Self::Item>;

    fn into_iter(self) -> Self::IntoIter {
        self.0.into_iter()
    }
}
