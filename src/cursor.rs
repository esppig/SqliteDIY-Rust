use crate::table::Table;
struct Cursor {
    table: Table,
    row_num: u32,
    eot: bool, // end of table 最后一个元素之后的位置
}
