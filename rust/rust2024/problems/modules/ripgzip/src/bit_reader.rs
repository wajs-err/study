#![forbid(unsafe_code)]

use std::{cmp::min, io::{self, BufRead}};


#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct BitSequence {
    bits: u32,
    len: u8,
}

impl BitSequence {
    pub fn new(bits: u32, len: u8) -> Self {
        Self { bits: bits & (1 << len) - 1, len }
    }

    pub fn bits(&self) -> u32 {
        self.bits
    }

    #[allow(dead_code)]
    pub fn len(&self) -> u8 {
        self.len
    }

    #[allow(dead_code, unused)]
    pub fn concat(self, other: Self) -> Self {
        unimplemented!()
    }
}

struct SingleByteBuff {
    byte: u8,
    len: u8,
}

pub struct BitReader<T> {
    stream: T,
    buff: Option<SingleByteBuff>,
}

impl<T: BufRead> BitReader<T> {
    pub fn new(stream: T) -> Self {
        Self { stream, buff: None }
    }

    pub fn read_bits(&mut self, mut len: u8) -> io::Result<BitSequence> {
        if len > 32 {
            return Err(io::Error::new(io::ErrorKind::InvalidInput, "too many bits requested"));
        }

        let mut bits = 0u32;
        let result_len = len;
        
        if let Some(buff) = &mut self.buff {
            let from_buff_len = min(len, buff.len);
            bits |= (buff.byte & (1 << from_buff_len) - 1) as u32;
            buff.byte >>= from_buff_len;
            buff.len -= from_buff_len;
            len -= from_buff_len;
            if buff.len == 0 {
                self.buff = None;
            }
        }

        while len != 0 {
            let byte = *self.stream.fill_buf()?.get(0).ok_or(io::Error::new(io::ErrorKind::UnexpectedEof, "unexpected eof"))?;
            self.stream.consume(1);
            let curr_len = min(8, len);
            bits |= ((byte & ((1 << (curr_len as u16)) - 1) as u8) as u32) << (result_len - len);
            len -= curr_len;
            if curr_len < 8 {
                self.buff = Some(SingleByteBuff {
                    byte: byte >> curr_len,
                    len: 8 - curr_len,
                });
            }
        }

        Ok(BitSequence::new(bits, result_len))
    }

    /// Discard all the unread bits in the current byte and return a mutable reference
    /// to the underlying reader.
    #[allow(dead_code)]
    pub fn borrow_reader_from_boundary(&mut self) -> &mut T {
        self.buff = None;
        &mut self.stream
    }
}


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
    fn read_u16() -> io::Result<()> {
        let data: &[u8] = &[0b01100011, 0b11011011, 0b10101111];
        let mut reader = BitReader::new(data);
        assert_eq!(reader.read_bits(1)?, BitSequence::new(0b1, 1));
        assert_eq!(reader.read_bits(16)?, BitSequence::new(0b1110110110110001, 16));
        assert_eq!(
            reader.read_bits(8).unwrap_err().kind(),
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
