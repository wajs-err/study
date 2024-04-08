#![forbid(unsafe_code)]

use std::collections::{BTreeMap, HashMap};
use std::hash::Hash;

#[derive(Debug)]
struct KeyValue<K, V> {
    key: K,
    value: V,
}

#[derive(Debug)]
pub struct LRUCache<K, V> {
    cap: usize,
    min_key: i64,
    map: HashMap<K, i64>,
    values: BTreeMap<i64, KeyValue<K, V>>,
}

impl<K: Clone + Hash + Ord, V> LRUCache<K, V> {
    pub fn new(capacity: usize) -> Self {
        if capacity == 0 {
            panic!();
        }
        LRUCache {
            cap: capacity,
            min_key: i64::MAX,
            map: HashMap::new(),
            values: BTreeMap::new(),
        }
    }

    pub fn get(&mut self, key: &K) -> Option<&V> {
        if let Some(i) = self.map.get_mut(key) {
            match self.values.remove(i) {
                Some(kv) => {
                    self.min_key -= 1;
                    *i = self.min_key;
                    self.values.insert(self.min_key, kv);
                    return self
                        .values
                        .get(&self.min_key)
                        .map(|kv: &KeyValue<K, V>| &kv.value);
                }
                None => unreachable!(),
            }
        }
        None
    }

    pub fn insert(&mut self, key: K, value: V) -> Option<V> {
        if let Some(i) = self.map.get_mut(&key) {
            let kv = match self.values.remove(i) {
                Some(kv) => kv,
                None => unreachable!(),
            };
            self.min_key -= 1;
            *i = self.min_key;
            self.values.insert(self.min_key, KeyValue { key, value });
            Some(kv.value)
        } else {
            if self.cap == self.map.len() {
                match self.values.pop_last() {
                    Some((_, kv)) => self.map.remove(&kv.key),
                    None => unreachable!(),
                };
            }
            self.min_key -= 1;
            self.map.insert(key.clone(), self.min_key);
            self.values.insert(self.min_key, KeyValue { key, value });
            None
        }
    }
}
