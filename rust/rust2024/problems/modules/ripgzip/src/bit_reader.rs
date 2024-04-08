#![forbid(unsafe_code)]

use std::{io::{self, BufRead}, usize};

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct BitSequence {
    bits: u16,
    len: u8,
}

impl BitSequence {
    pub fn new(bits: u16, len: u8) -> Self {
        Self { bits, len }
    }

    pub fn bits(&self) -> u16 {
        self.bits
    }

    pub fn len(&self) -> u8 {
        self.len
    }

    pub fn concat(self, other: Self) -> Self {
        Self {
            bits: self.bits | other.bits << other.len,
            len: self.len + other.len
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

pub struct BitReader<T> {
    stream: T,
    curr_bit: usize,
}

impl<T: BufRead> BitReader<T> {
    pub fn new(stream: T) -> Self {
        Self { stream, curr_bit: 0 }
    }

    pub fn read_bits(&mut self, mut len: u8) -> io::Result<BitSequence> {
        dbg!(len);
        let (curr_byte, bit_in_byte) = (self.curr_bit / 8, self.curr_bit % 8);
        dbg!(curr_byte, bit_in_byte);
        let mut bits = (self.stream.fill_buf()?.get(curr_byte).ok_or(io::Error::new(io::ErrorKind::UnexpectedEof, "Unexpected end of block"))? >> bit_in_byte & (((1u16 << len as u16) - 1)) as u8) as u16;
        if len as usize > 8 - bit_in_byte {
            self.curr_bit += 8 - bit_in_byte;
            bits |= self.read_bits(len + bit_in_byte as u8 - 8)?.bits() << 8 - bit_in_byte as u8;
        } else {
            self.curr_bit += len as usize;
        }
        Ok(BitSequence::new(bits, len))
    }

    pub fn borrow_reader_from_boundary(&mut self) -> &mut T {
        let curr_byte = self.curr_bit / 8;
        self.curr_bit = 0;
        self.stream.consume(curr_byte + 1);
        &mut self.stream
    }
}

////////////////////////////////////////////////////////////////////////////////

#[cfg(test)]
mod tests {
    use super::*;
    use byteorder::ReadBytesExt;

    #[test]
    fn read_bits() -> io::Result<()> {
        let data: &[u8] = &[0b01100011, 0b11011011, 0b10101111];
        let mut reader = BitReader::new(data);
        assert_eq!(reader.read_bits(1)?, BitSequence::new(0b1, 1));
        assert_eq!(reader.read_bits(2)?, BitSequence::new(0b01, 2));
        assert_eq!(reader.read_bits(3)?, BitSequence::new(0b100, 3));
        assert_eq!(reader.read_bits(4)?, BitSequence::new(0b1101, 4));
        assert_eq!(reader.read_bits(5)?, BitSequence::new(0b10110, 5));
        assert_eq!(reader.read_bits(8)?, BitSequence::new(0b01011111, 8));
        assert_eq!(
            reader.read_bits(2).unwrap_err().kind(),
            io::ErrorKind::UnexpectedEof
        );
        Ok(())
    }

    #[test]
    fn borrow_reader_from_boundary() -> io::Result<()> {
        let data: &[u8] = &[0b01100011, 0b11011011, 0b10101111];
        let mut reader = BitReader::new(data);
        assert_eq!(reader.read_bits(3)?, BitSequence::new(0b011, 3));
        assert_eq!(reader.borrow_reader_from_boundary().read_u8()?, 0b11011011);
        assert_eq!(reader.read_bits(8)?, BitSequence::new(0b10101111, 8));
        Ok(())
    }
}
