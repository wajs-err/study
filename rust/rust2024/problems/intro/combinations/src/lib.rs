#![forbid(unsafe_code)]

fn combinations_impl(arr: &[i32], result: &mut Vec<Vec<i32>>, k: usize, from: usize) {
    let last = match result.last() {
        Some(last) => {
            if last.len() == k {
                result.push(last[0..last.len() - 1].to_vec());
                return;
            }
            last
        }
        None => unreachable!(),
    };

    let remain = k - last.len();
    for i in from..=arr.len() - remain {
        if let Some(l) = result.last_mut() {
            l.push(arr[i]);
        }
        combinations_impl(arr, result, k, i + 1);
    }
    if let Some(l) = result.last_mut() {
        l.pop();
    }
}

pub fn combinations(arr: &[i32], k: usize) -> Vec<Vec<i32>> {
    let mut result = Vec::new();
    if k > arr.len() {
        return result;
    }

    result.push(Vec::new());
    if k != 0 {
        combinations_impl(arr, &mut result, k, 0);
        result.pop();
    }
    result
}
