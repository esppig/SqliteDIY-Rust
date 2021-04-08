pub struct Row {
    id: u32,
    name: [u8; 32],
    email: [u8; 255],
}

impl Row {
    pub fn new() -> Row {
        Row {
            id: 0,
            name: [0; 32],
            email: [0; 255],
        }
    }

    pub fn serialize_row(&self) {}
}

// 硬编码一张表
// Table(id u32, username u8 * 32,  email u8 * 255);
// SIZE(id 4bytes, username 32bytes, email 255bytes);
// OFFSET(id 0, username 4, email 36);

const ID_SIZE: u32 = 4;
const USERNAME_SIZE: u32 = 32;
const EMAIL_SIZE: u32 = 255;
const ROW_SIZE: u32 = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const ID_OFFSET: u32 = 0;
const USERNAME_OFFSET: u32 = ID_OFFSET + ID_SIZE;
const EMAIL_OFFSET: u32 = USERNAME_OFFSET + USERNAME_SIZE;

const PAGE_SIZE: u32 = 4096;
const TABLE_MAX_PAGES: u32 = 100;
const ROWS_PER_PAGE: u32 = PAGE_SIZE / ROW_SIZE;
const TABLE_MAX_ROWS: u32 = ROWS_PER_PAGE * TABLE_MAX_PAGES;

pub struct Table {
    rows_count: u32,
    pages: Vec<Vec<u8>>,
}

impl Table {
    fn row_slot(&mut self, row_count: u32) -> (u32, u32) {
        let page_num = row_count / ROWS_PER_PAGE;
        let row_offset = row_count / ROWS_PER_PAGE;
        let byte_offset = row_offset / ROW_SIZE;
        // let page = &self.pages[page_num];
        &mut self.get_page(page_num);
        return (page_num, byte_offset);
    }

    fn get_page(&mut self, page_num: u32) {
        if page_num > TABLE_MAX_PAGES {
            println!(
                "page number out of bounds. {} > {}",
                page_num, TABLE_MAX_PAGES
            );
            std::process::exit(1);
        }

        if self.pages[page_num as usize].is_empty() {
            let page = vec![0u8; PAGE_SIZE as usize];
            self.pages[page_num as usize].extend_from_slice(page.as_slice());
        }
    }
}
