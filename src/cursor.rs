use crate::table::Table;
pub struct Cursor<'a> {
    pub table: &'a mut Table,
    pub row_num: u32,
    pub end_of_table: bool, // end of table 最后一个元素之后的位置
}

impl<'a> Cursor<'a> {
    pub fn table_start(table: &mut Table) -> Cursor {
        let eot = table.rows_count == 0;
        Cursor {
            table,
            row_num: 0,
            end_of_table: eot,
        }
    }

    pub fn table_end(table: &mut Table) -> Cursor {
        let cnt = table.rows_count;
        Cursor {
            table,
            row_num: cnt,
            end_of_table: true,
        }
    }

    pub fn cursor_advance(&mut self) {
        self.row_num += 1;
        if self.row_num >= self.table.rows_count {
            self.end_of_table = true;
        }
    }
}
