use crate::constants::*;
use core::num;
use std::io::{Read, Seek, SeekFrom};
use std::{
    fs::{File, OpenOptions},
    io::Write,
};

#[derive(Debug)]
pub struct Row {
    pub id: u32,
    pub name: [u8; 32],
    pub email: [u8; 255],
}

impl Row {
    pub fn new() -> Row {
        Row {
            id: 0,
            name: [0; USERNAME_SIZE],
            email: [0; EMAIL_SIZE],
        }
    }

    pub fn show(&self) {
        println!(
            "({},{},{})",
            self.id,
            std::str::from_utf8(&self.name)
                .unwrap()
                .trim_end_matches(char::from(0)),
            std::str::from_utf8(&self.email)
                .unwrap()
                .trim_end_matches(char::from(0))
        );
    }
}

struct Pager {
    file: File,          // DB文件
    file_length: u64,    // 文件长度
    pages: Vec<Vec<u8>>, // 数据页
}

impl Pager {
    fn new() -> Pager {
        Pager {
            file: File::create("xx").unwrap(),
            file_length: 0,
            pages: vec![vec![]; TABLE_MAX_PAGES],
        }
    }

    fn new_from_file(filename: &str) -> Pager {
        match OpenOptions::new()
            .read(true)
            .write(true)
            .create(true)
            .open(filename)
        {
            Ok(mut file) => {
                let file_length = file.seek(SeekFrom::End(0)).unwrap();
                println!("file length is {}", file_length);
                Pager {
                    file,
                    file_length,
                    pages: vec![vec![]; TABLE_MAX_PAGES],
                }
            }
            Err(e) => {
                println!("open file err {}", e);
                std::process::exit(1);
            }
        }
    }

    fn get_page(&mut self, page_num: u32) {
        if page_num > TABLE_MAX_PAGES as u32 {
            println!(
                "page number out of bounds. {} > {}",
                page_num, TABLE_MAX_PAGES
            );
            std::process::exit(1);
        }
        if self.pages[page_num as usize].is_empty() {
            // println!("page-> {} empty!", page_num);

            let mut nums_pages = self.file_length / PAGE_SIZE as u64;

            if self.file_length % PAGE_SIZE as u64 > 0 {
                nums_pages += 1;
            }

            if page_num as u64 <= nums_pages {
                if self
                    .file
                    .seek(SeekFrom::Start((page_num * PAGE_SIZE as u32) as u64))
                    .is_err()
                {
                    println!("seeking file err");
                    std::process::exit(1);
                }
                let buf_size = if self.file_length >= (page_num * PAGE_SIZE as u32) as u64 {
                    (self.file_length - (page_num * PAGE_SIZE as u32) as u64) as usize
                } else {
                    // PAGE_SIZE
                    // self.file_length as usize
                    PAGE_SIZE
                };
                let mut page = vec![0u8; buf_size];

                if self.file.read_exact(page.as_mut_slice()).is_err() {
                    println!("error reading file {}", page.len());
                    std::process::exit(1);
                }
                // TODO
                // self.pages[page_num as usize].extend(page.as_slice());
                self.pages[page_num as usize].extend_from_slice(page.as_slice());
            }
        } else {
            // println!("page-> {} alloced!", page_num);
        }
    }

    fn flush(&mut self, page_num: u32) {
        if self.pages[page_num as usize].is_empty() {
            println!("Tried to flush null page");
            std::process::exit(1);
        }

        if self
            .file
            .seek(SeekFrom::Start((page_num * PAGE_SIZE as u32) as u64))
            .is_err()
        {
            println!("Error seeking.");
            std::process::exit(1);
        }

        let drained_vec: Vec<u8> = self.pages[page_num as usize].drain(..).collect();
        self.pages[page_num as usize].shrink_to_fit();

        if self.file.write_all(drained_vec.as_ref()).is_err() {
            println!("Error writing.");
            std::process::exit(1);
        }
    }
}

pub struct Table {
    pub rows_count: u32,
    // pages: Vec<Vec<u8>>,
    pager: Pager,
}

impl Table {
    pub fn new() -> Table {
        Table {
            rows_count: 0,
            // pages: vec![vec![]; TABLE_MAX_PAGES as usize],
            pager: Pager::new(),
        }
    }

    pub fn db_open(filename: &str) -> Table {
        let pager = Pager::new_from_file(filename);
        let rows_count = (pager.file_length / ROW_SIZE as u64) as u32;
        Table { rows_count, pager }
    }

    pub fn row_slot(&mut self, row_count: u32) -> (u32, u32) {
        let page_num = row_count / ROWS_PER_PAGE as u32;
        let row_offset = row_count % ROWS_PER_PAGE as u32;
        let byte_offset = row_offset * ROW_SIZE as u32;
        // println!(
        //     "slot: page_num={}, row_offset={}, byte_offset={}",
        //     page_num, row_offset, byte_offset
        // );
        self.pager.get_page(page_num);
        return (page_num, byte_offset);
    }

    pub fn serialize_row(&mut self, row: &Row, page_num: u32, byte_offsets: u32) {
        let id_bytes = row.id.to_ne_bytes();
        let name_bytes = row.name;
        let email_bytes = row.email;

        // println!("{:?}, {:?}, {:?}", &id_bytes, &name_bytes, &email_bytes);

        let offset = byte_offsets as usize;

        // self.pager.pages[page_num as usize][(offset + ID_OFFSET)..(offset + ID_OFFSET + ID_SIZE)]
        //     .copy_from_slice(&id_bytes);
        // self.pager.pages[page_num as usize]
        //     [(offset + USERNAME_OFFSET)..(offset + USERNAME_OFFSET + USERNAME_SIZE)]
        //     .copy_from_slice(&name_bytes);
        // self.pager.pages[page_num as usize]
        //     [(offset + EMAIL_OFFSET)..(offset + EMAIL_OFFSET + EMAIL_SIZE)]
        //     .copy_from_slice(&email_bytes);

        self.pager.pages[page_num as usize].extend_from_slice(&id_bytes);
        self.pager.pages[page_num as usize].extend_from_slice(&name_bytes);
        self.pager.pages[page_num as usize].extend_from_slice(&email_bytes);

        // println!("{:?}", &self.pager.pages[page_num as usize]);
    }

    pub fn deserialize_row(&mut self, page_num: u32, byte_offsets: u32) -> Row {
        let offset = byte_offsets as usize;
        let id_bytes_slice = &self.pager.pages[page_num as usize]
            [(offset + ID_OFFSET)..(offset + ID_OFFSET + ID_SIZE)];
        let name_bytes_slice = &self.pager.pages[page_num as usize]
            [(offset + USERNAME_OFFSET)..(offset + USERNAME_OFFSET + USERNAME_SIZE)];
        let email_bytes_slice = &self.pager.pages[page_num as usize]
            [(offset + EMAIL_OFFSET)..(offset + EMAIL_OFFSET + EMAIL_SIZE)];

        let mut id_bytes_arr = [0; 4];
        id_bytes_arr.copy_from_slice(id_bytes_slice);
        let id = u32::from_ne_bytes(id_bytes_arr);
        let mut name = [0u8; USERNAME_SIZE];
        name.copy_from_slice(name_bytes_slice);
        let mut email = [0u8; EMAIL_SIZE];
        email.copy_from_slice(email_bytes_slice);

        Row { id, name, email }
    }

    pub fn db_close(&mut self) {
        let num_full_pages = self.rows_count / ROWS_PER_PAGE as u32;

        for i in 0..num_full_pages {
            if self.pager.pages[i as usize].is_empty() {
                continue;
            }
            self.pager.flush(i);
        }

        let num_add_rows = self.rows_count % ROWS_PER_PAGE as u32;
        if num_add_rows > 0 {
            let page_num = num_full_pages;
            if !self.pager.pages[page_num as usize].is_empty() {
                self.pager.flush(page_num);
            }
        }

        if self.pager.file.sync_data().is_err() {
            println!("error closing db file.");
            std::process::exit(1);
        }
    }
}
