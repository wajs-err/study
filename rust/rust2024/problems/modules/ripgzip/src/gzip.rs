#![forbid(unsafe_code)]

use std::{error::Error, io::{self, BufRead, Write}};

use anyhow::{anyhow, bail, ensure, Context, Result};
use byteorder::{LittleEndian, ReadBytesExt};
use crc::Crc;

use crate::{
    bit_reader::BitReader,
    deflate::{self, DeflateReader},
    tracking_writer::TrackingWriter,
};


#[allow(dead_code)]
const ID1: u8 = 0x1f;
#[allow(dead_code)]
const ID2: u8 = 0x8b;

const CM_DEFLATE: u8 = 8;

const FTEXT_OFFSET: u8 = 0;
const FHCRC_OFFSET: u8 = 1;
const FEXTRA_OFFSET: u8 = 2;
const FNAME_OFFSET: u8 = 3;
const FCOMMENT_OFFSET: u8 = 4;


#[derive(Debug)]
pub struct MemberHeader {
    pub compression_method: CompressionMethod,
    pub modification_time: u32,
    pub extra: Option<Vec<u8>>,
    pub name: Option<String>,
    pub comment: Option<String>,
    pub extra_flags: u8,
    pub os: u8,
    pub has_crc: bool,
    pub is_text: bool,
}

impl MemberHeader {
    #[allow(dead_code)]
    pub fn crc16(&self) -> u16 {
        let crc = Crc::<u32>::new(&crc::CRC_32_ISO_HDLC);
        let mut digest = crc.digest();

        digest.update(&[ID1, ID2, self.compression_method.into(), self.flags().0]);
        digest.update(&self.modification_time.to_le_bytes());
        digest.update(&[self.extra_flags, self.os]);

        if let Some(extra) = &self.extra {
            digest.update(&(extra.len() as u16).to_le_bytes());
            digest.update(extra);
        }

        if let Some(name) = &self.name {
            digest.update(name.as_bytes());
            digest.update(&[0]);
        }

        if let Some(comment) = &self.comment {
            digest.update(comment.as_bytes());
            digest.update(&[0]);
        }

        (digest.finalize() & 0xffff) as u16
    }

    #[allow(dead_code)]
    pub fn flags(&self) -> MemberFlags {
        let mut flags = MemberFlags(0);
        flags.set_is_text(self.is_text);
        flags.set_has_crc(self.has_crc);
        flags.set_has_extra(self.extra.is_some());
        flags.set_has_name(self.name.is_some());
        flags.set_has_comment(self.comment.is_some());
        flags
    }
}


#[derive(Clone, Copy, Debug)]
pub enum CompressionMethod {
    Deflate,
    Unknown(u8),
}

impl From<u8> for CompressionMethod {
    fn from(value: u8) -> Self {
        match value {
            CM_DEFLATE => Self::Deflate,
            x => Self::Unknown(x),
        }
    }
}

impl From<CompressionMethod> for u8 {
    fn from(method: CompressionMethod) -> u8 {
        match method {
            CompressionMethod::Deflate => CM_DEFLATE,
            CompressionMethod::Unknown(x) => x,
        }
    }
}


#[derive(Debug)]
pub struct MemberFlags(u8);

#[allow(unused)]
impl MemberFlags {
    fn bit(&self, n: u8) -> bool {
        (self.0 >> n) & 1 != 0
    }

    fn set_bit(&mut self, n: u8, value: bool) {
        if value {
            self.0 |= 1 << n;
        } else {
            self.0 &= !(1 << n);
        }
    }

    pub fn is_text(&self) -> bool {
        self.bit(FTEXT_OFFSET)
    }

    pub fn set_is_text(&mut self, value: bool) {
        self.set_bit(FTEXT_OFFSET, value)
    }

    pub fn has_crc(&self) -> bool {
        self.bit(FHCRC_OFFSET)
    }

    pub fn set_has_crc(&mut self, value: bool) {
        self.set_bit(FHCRC_OFFSET, value)
    }

    pub fn has_extra(&self) -> bool {
        self.bit(FEXTRA_OFFSET)
    }

    pub fn set_has_extra(&mut self, value: bool) {
        self.set_bit(FEXTRA_OFFSET, value)
    }

    pub fn has_name(&self) -> bool {
        self.bit(FNAME_OFFSET)
    }

    pub fn set_has_name(&mut self, value: bool) {
        self.set_bit(FNAME_OFFSET, value)
    }

    pub fn has_comment(&self) -> bool {
        self.bit(FCOMMENT_OFFSET)
    }

    pub fn set_has_comment(&mut self, value: bool) {
        self.set_bit(FCOMMENT_OFFSET, value)
    }
}


#[derive(Debug)]
pub struct MemberFooter {
    pub data_crc32: u32,
    pub data_size: u32,
}

#[allow(dead_code)]
pub struct GzipReader<T> {
    reader: T,
}

impl<T: BufRead> GzipReader<T> {
    #[allow(dead_code)]
    pub fn new(reader: T) -> Self {
        Self { reader }
    }

    #[allow(dead_code)]
    fn parse_header(mut header: &[u8]) -> Result<(MemberHeader, MemberFlags)> {
        let mut bit_reader = BitReader::new(header);

        let id1 = bit_reader.read_bits(8)?.bits() as u8;
        if id1 != ID1 {
            return Err(anyhow!("wrong gzip header"));
        }
        let id2 = bit_reader.read_bits(8)?.bits() as u8;
        if id2 != ID2 {
            return Err(anyhow!("wrong gzip header"));
        }

        let cm = match bit_reader.read_bits(8)?.bits() as u8 {
            CM_DEFLATE => CompressionMethod::Deflate,
            u => CompressionMethod::Unknown(u),
        };
        let flg = MemberFlags(bit_reader.read_bits(8)?.bits() as u8);
        let mtime = bit_reader.read_bits(32)?.bits() as u32;
        let xfl = bit_reader.read_bits(8)?.bits() as u8;
        let os = bit_reader.read_bits(8)?.bits() as u8;

        let mut member_header = MemberHeader {
            compression_method: cm,
            modification_time: mtime,
            extra: None,
            name: None,
            comment: None,
            extra_flags: xfl,
            os,
            has_crc: flg.has_crc(),
            is_text: flg.is_text(),
        };
        
        if flg.has_extra() {
            let xlen = bit_reader.read_bits(16)?.bits() as usize;
            // TODO : can be done without allocations?
            member_header.extra = Some(header[..xlen].to_vec());
            header.consume(xlen);
        }
        if flg.has_name() {
            let name = String::new();
            header.read_until(0x00, &mut name.as_bytes().to_owned())?;
            member_header.name = Some(name); // TODO
        }
        if flg.has_comment() {
            let comment = String::new();
            header.read_until(0x00, &mut comment.as_bytes().to_owned())?;
            member_header.comment = Some(comment); // TODO
        }
        if flg.has_crc() {
            let crc16 = bit_reader.read_bits(16)?.bits() as u16;
            if member_header.crc16() != crc16 {
                return Err(anyhow!("crc16 differs"));
            }
        }

        Ok((member_header, flg))
    }

    // TODO: your code goes here.
}


#[allow(dead_code)]
pub struct MemberReader<T> {
    inner: T,
}

impl<T: BufRead> MemberReader<T> {
    #[allow(dead_code)]
    pub fn inner_mut(&mut self) -> &mut T {
        &mut self.inner
    }

    #[allow(dead_code)]
    pub fn read_footer(mut self) -> Result<(MemberFooter, GzipReader<T>)> {
        unimplemented!()
    }
}
