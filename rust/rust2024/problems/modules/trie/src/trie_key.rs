#![forbid(unsafe_code)]
use std::str::Chars;

pub trait ToKeyIter {
    type Item;
    type KeyIter<'a>: Iterator<Item = Self::Item>
    where
        Self: 'a;

    fn key_iter(&self) -> Self::KeyIter<'_>;
}

impl ToKeyIter for str {
    type Item = char;
    type KeyIter<'a> = Chars<'a>;

    fn key_iter(&self) -> Self::KeyIter<'_> {
        self.chars()
    }
}

impl ToKeyIter for String {
    type Item = char;
    type KeyIter<'a> = Chars<'a>;

    fn key_iter(&self) -> Self::KeyIter<'_> {
        self.chars()
    }
}

////////////////////////////////////////////////////////////////////////////////

// Bonus

// pub trait FromKeyIter {
//     fn to_key(self) -> ???;
// }

// impl FromKeyIter for ???
// TODO: your code goes here.

////////////////////////////////////////////////////////////////////////////////

// Bonus

// pub trait TrieKey
// TODO: your code goes here.
