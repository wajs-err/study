#![forbid(unsafe_code)]

fn shorten_prefix(prefix: &str, s: &str) -> usize {
    let mut len = 0;
    for (c_0, c_1) in std::iter::zip(prefix.char_indices(), s.char_indices()) {
        if c_0 == c_1 {
            len = c_1.0;
        } else {
            break;
        }
    }
    len
}

pub fn longest_common_prefix(strs: Vec<&str>) -> String {
    match strs.len() {
        0 => String::new(),
        _ => {
            let prefix = strs[0];
            let mut len = prefix.len();
            for s in strs.iter().skip(1) {
                len = shorten_prefix(prefix, s)
            }
            if len != 0 {
                prefix[0..=len].to_string()
            } else {
                String::from("")
            }
        }
    }
}
