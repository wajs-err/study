#![forbid(unsafe_code)]
use crate::trie_key::ToKeyIter;
use std::{
    collections::HashMap,
    hash::Hash,
    mem::swap,
    ops::{Deref, DerefMut, Index},
};

struct TrieNode<K, V> {
    children: HashMap<K, Box<TrieNode<K, V>>>,
    value: Option<V>,
}

impl<K, V> TrieNode<K, V> {
    pub fn new() -> Self {
        Self {
            children: HashMap::new(),
            value: None,
        }
    }

    pub fn set_value(&mut self, value: V) -> Option<V> {
        let mut v = Some(value);
        swap(&mut v, &mut self.value);
        v
    }
}

impl<'a, K, V> Default for Trie<'a, K, V>
where
    K: ToKeyIter + 'a,
    K::KeyIter<'a>: Iterator<Item = <K as ToKeyIter>::Item>,
    <K::KeyIter<'a> as Iterator>::Item: Clone + Eq + Hash,
{
    fn default() -> Self {
        Self::new()
    }
}

pub struct Trie<'a, K, V>
where
    K: ToKeyIter + 'a,
    K::KeyIter<'a>: Iterator<Item = <K as ToKeyIter>::Item>,
    <K::KeyIter<'a> as Iterator>::Item: Clone + Eq + Hash,
{
    root: TrieNode<<<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item, V>,
    len: usize,
}

impl<'a, K, V> Trie<'a, K, V>
where
    K: ToKeyIter + 'a,
    K::KeyIter<'a>: Iterator<Item = <K as ToKeyIter>::Item>,
    <K::KeyIter<'a> as Iterator>::Item: Clone + Eq + Hash,
{
    pub fn new() -> Self {
        Self {
            root: TrieNode::<<<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item, V>::new(),
            len: 0,
        }
    }

    pub fn len(&self) -> usize {
        self.len
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    pub fn insert<'b, Q>(&mut self, key: &'b Q, value: V) -> Option<V>
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
    {
        let mut node = &mut self.root;
        for k in key.key_iter() {
            if !node.children.contains_key(&k) {
                node.children.insert(k.clone(), Box::new(TrieNode::new()));
            }
            node = node.children.get_mut(&k).unwrap().deref_mut();
        }
        match node.set_value(value) {
            Some(v) => Some(v),
            None => {
                self.len += 1;
                None
            }
        }
    }

    fn get_node<'b, Q>(
        &self,
        key: &'b Q,
    ) -> Option<&TrieNode<<<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item, V>>
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
        <Q::KeyIter<'b> as Iterator>::Item: Eq + Hash,
    {
        let mut node = &self.root;
        for k in key.key_iter() {
            if let Some(child) = node.children.get(&k) {
                node = child.deref();
            } else {
                return None;
            }
        }
        Some(node)
    }

    pub fn get<'b, Q>(&self, key: &'b Q) -> Option<&V>
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
        <Q::KeyIter<'b> as Iterator>::Item: Eq + Hash,
    {
        self.get_node(key)?.value.as_ref()
    }

    pub fn get_mut<'b, Q>(&mut self, key: &'b Q) -> Option<&mut V>
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
        <Q::KeyIter<'b> as Iterator>::Item: Clone + Eq + Hash,
    {
        let mut node = &mut self.root;
        for k in key.key_iter() {
            if let Some(child) = node.children.get_mut(&k) {
                node = child.deref_mut();
            } else {
                return None;
            }
        }
        node.value.as_mut()
    }

    pub fn contains<'b, Q>(&self, key: &'b Q) -> bool
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
        <Q::KeyIter<'b> as Iterator>::Item: Eq + Hash,
    {
        self.get(key).is_some()
    }

    pub fn starts_with<'b, Q>(&self, key: &'b Q) -> bool
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
        <Q::KeyIter<'b> as Iterator>::Item: Eq + Hash,
    {
        self.get_node(key).is_some()
    }

    fn remove_impl<'b, Q>(
        node: &mut TrieNode<<<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item, V>,
        key: &mut <Q as ToKeyIter>::KeyIter<'b>,
    ) -> (
        Option<V>,
        Option<<<Q as ToKeyIter>::KeyIter<'b> as Iterator>::Item>,
        bool,
    )
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
        <Q::KeyIter<'b> as Iterator>::Item: Eq + Hash,
    {
        if let Some(k) = key.next() {
            match node.children.get_mut(&k) {
                Some(node) => {
                    let (v, p, mut need_remove) = Trie::<'a, K, V>::remove_impl::<Q>(node, key);
                    if need_remove {
                        if let Some(q) = p {
                            node.children.remove(&q);
                        }
                    }
                    if node.value.is_some() || !node.children.is_empty() {
                        need_remove = false;
                    }
                    (v, Some(k), need_remove)
                }
                None => (None, Some(k), false),
            }
        } else {
            let mut v = None;
            swap(&mut v, &mut node.value);
            (v, None, node.children.is_empty())
        }
    }

    pub fn remove<'b, Q>(&mut self, key: &'b Q) -> Option<V>
    where
        Q: ?Sized + ToKeyIter,
        Q::KeyIter<'b>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
        <Q::KeyIter<'b> as Iterator>::Item: Eq + Hash,
    {
        let node = &mut self.root;
        let (v, p, need_remove) = Trie::<'a, K, V>::remove_impl::<Q>(node, &mut key.key_iter());
        if need_remove {
            if let Some(q) = p {
                node.children.remove(&q);
            }
        }
        v.map(|v| {
            self.len -= 1;
            v
        })
    }
}

impl<'a, K: ToKeyIter + 'a, Q: ?Sized + ToKeyIter + 'a, V> Index<&'a Q> for Trie<'a, K, V>
where
    K::KeyIter<'a>: Iterator,
    <K::KeyIter<'a> as Iterator>::Item: Clone + Eq + Hash,
    Q::KeyIter<'a>: Iterator<Item = <<K as ToKeyIter>::KeyIter<'a> as Iterator>::Item>,
    <Q::KeyIter<'a> as Iterator>::Item: Eq + Hash,
{
    type Output = V;

    fn index(&self, index: &'a Q) -> &Self::Output {
        match self.get(index) {
            Some(value) => value,
            None => panic!("no such key"),
        }
    }
}
