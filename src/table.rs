use crate::constants::*;

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

pub struct Table {
    pub rows_count: u32,
    pages: Vec<Vec<u8>>,
}

impl Table {
    pub fn new() -> Table {
        Table {
            rows_count: 0,
            pages: vec![vec![]; TABLE_MAX_PAGES as usize],
        }
    }

    pub fn row_slot(&mut self, row_count: u32) -> (u32, u32) {
        let page_num = row_count / ROWS_PER_PAGE as u32;
        let row_offset = row_count % ROWS_PER_PAGE as u32;
        let byte_offset = row_offset * ROW_SIZE as u32;
        println!(
            "slot: page_num={}, row_offset={}, byte_offset={}",
            page_num, row_offset, byte_offset
        );
        self.get_page(page_num);
        return (page_num, byte_offset);
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
            println!("page-> {} empty!", page_num);
            let page = vec![0u8; PAGE_SIZE as usize];
            self.pages[page_num as usize].extend_from_slice(page.as_slice());
        } else {
            println!("page-> {} alloced!", page_num);
        }
    }

    pub fn serialize_row(&mut self, row: &Row, page_num: u32, byte_offsets: u32) {
        let id_bytes = row.id.to_ne_bytes();
        let name_bytes = row.name;
        let email_bytes = row.email;

        println!("{:?}, {:?}, {:?}", &id_bytes, &name_bytes, &email_bytes);

        let offset = byte_offsets as usize;

        self.pages[page_num as usize][(offset + ID_OFFSET)..(offset + ID_OFFSET + ID_SIZE)]
            .copy_from_slice(&id_bytes);
        self.pages[page_num as usize]
            [(offset + USERNAME_OFFSET)..(offset + USERNAME_OFFSET + USERNAME_SIZE)]
            .copy_from_slice(&name_bytes);
        self.pages[page_num as usize]
            [(offset + EMAIL_OFFSET)..(offset + EMAIL_OFFSET + EMAIL_SIZE)]
            .copy_from_slice(&email_bytes);

        // self.pages[page_num as usize].extend_from_slice(&id_bytes);
        // self.pages[page_num as usize].extend_from_slice(&name_bytes);
        // self.pages[page_num as usize].extend_from_slice(&email_bytes);

        println!("{:?}", &self.pages[page_num as usize]);
    }

    pub fn deserialize_row(&mut self, page_num: u32, byte_offsets: u32) -> Row {
        let offset = byte_offsets as usize;
        let id_bytes_slice =
            &self.pages[page_num as usize][(offset + ID_OFFSET)..(offset + ID_OFFSET + ID_SIZE)];
        let name_bytes_slice = &self.pages[page_num as usize]
            [(offset + USERNAME_OFFSET)..(offset + USERNAME_OFFSET + USERNAME_SIZE)];
        let email_bytes_slice = &self.pages[page_num as usize]
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
}
