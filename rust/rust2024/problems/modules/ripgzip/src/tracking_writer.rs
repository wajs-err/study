#![forbid(unsafe_code)]

use std::collections::VecDeque;
use std::io::{self, Read, Write};

use anyhow::{anyhow, ensure, Error, Result};
use crc::{Crc, Digest, CRC_32_ISO_HDLC};

////////////////////////////////////////////////////////////////////////////////

const HISTORY_SIZE: usize = 32768;

pub struct TrackingWriter<T> {
    inner: T,
    buff: VecDeque<u8>,
    written: usize,
}

impl<T: Write> Write for TrackingWriter<T> {
    fn write(&mut self, buff: &[u8]) -> io::Result<usize> {
        let written = self.inner.write(buff)?;
        for i in 0..written {
            self.buff.push_back(buff[i]);
        }
        // ???
        // while self.buff.len() > HISTORY_SIZE {
        //     self.buff.pop_front();
        // }
        self.written += written;
        Ok(written)
    }

    fn flush(&mut self) -> io::Result<()> {
        // ???
        self.buff = VecDeque::with_capacity(HISTORY_SIZE);
        self.written = 0;
        self.inner.flush()
    }
}

impl<T: Write> TrackingWriter<T> {
    pub fn new(inner: T) -> Self {
        Self { inner, buff: VecDeque::with_capacity(HISTORY_SIZE), written: 0 }
    }

    /// Write a sequence of `len` bytes written `dist` bytes ago.
    pub fn write_previous(&mut self, dist: usize, len: usize) -> Result<()> {
        dbg!(self.written);
        if dist > self.written || len > dist {
            return Err(Error::new(std::io::Error::new(std::io::ErrorKind::Other, "TODO")));
        }
        let from = self.buff.len() - dist;
        let to = from + len;
        // ???
        let written = self.inner.write(&self.buff.make_contiguous()[from..to])?;
        // ???
        for i in from..from + written {
            self.buff.push_back(self.buff[i]);
        }
        self.written += written;
        if written == len {
            Ok(())
        } else {
            Err(Error::new(std::io::Error::new(std::io::ErrorKind::Other, "TODO")))
        }
    }

    pub fn byte_count(&self) -> usize {
        self.written
    }

    pub fn crc32(mut self) -> u32 {
        let crc = Crc::<u32>::new(&CRC_32_ISO_HDLC);
        let mut digest = crc.digest();
        // ?????
        digest.update(&self.buff.make_contiguous()[..]);
        digest.finalize()
    }
}

////////////////////////////////////////////////////////////////////////////////

#[cfg(test)]
mod tests {
    use super::*;
    use byteorder::WriteBytesExt;

    #[test]
    fn write() -> Result<()> {
        let mut buf: &mut [u8] = &mut [0u8; 10];
        let mut writer = TrackingWriter::new(&mut buf);

        assert_eq!(writer.write(&[1, 2, 3, 4])?, 4);
        assert_eq!(writer.byte_count(), 4);

        assert_eq!(writer.write(&[4, 8, 15, 16, 23])?, 5);
        assert_eq!(writer.byte_count(), 9);

        assert_eq!(writer.write(&[0, 0, 123])?, 1);
        assert_eq!(writer.byte_count(), 10);

        assert_eq!(writer.write(&[42, 124, 234, 27])?, 0);
        assert_eq!(writer.byte_count(), 10);
        assert_eq!(writer.crc32(), 2992191065);

        Ok(())
    }

    #[test]
    fn write_previous() -> Result<()> {
        let mut buf: &mut [u8] = &mut [0u8; 512];
        let mut writer = TrackingWriter::new(&mut buf);

        for i in 0..=255 {
            writer.write_u8(i)?;
        }

        writer.write_previous(192, 128)?;
        assert_eq!(writer.byte_count(), 384);

        assert!(writer.write_previous(10000, 20).is_err());
        assert_eq!(writer.byte_count(), 384);

        assert!(writer.write_previous(256, 256).is_err());
        assert_eq!(writer.byte_count(), 512);

        assert!(writer.write_previous(1, 1).is_err());
        assert_eq!(writer.byte_count(), 512);
        assert_eq!(writer.crc32(), 2733545866);

        Ok(())
    }
}
