#![forbid(unsafe_code)]

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, PartialEq, Eq)]
pub struct Grid<T> {
    rows: usize,
    cols: usize,
    grid: Vec<T>,
}

impl<T: Clone + Default> Grid<T> {
    pub fn new(rows: usize, cols: usize) -> Self {
        Grid {
            rows,
            cols,
            grid: std::iter::repeat(T::default())
                .take(rows * cols)
                .collect::<Vec<_>>(),
        }
    }

    pub fn from_slice(grid: &[T], rows: usize, cols: usize) -> Self {
        Grid {
            rows,
            cols,
            grid: grid.to_vec(),
        }
    }

    pub fn size(&self) -> (usize, usize) {
        (self.rows, self.cols)
    }

    pub fn get(&self, row: usize, col: usize) -> &T {
        &self.grid[row * self.cols + col]
    }

    pub fn set(&mut self, value: T, row: usize, col: usize) {
        self.grid[row * self.cols + col] = value;
    }

    pub fn neighbours(&self, row: usize, col: usize) -> Vec<(usize, usize)> {
        let mut res = Vec::new();
        if row != 0 && col != 0 {
            res.push((row - 1, col - 1))
        }
        if row != 0 {
            res.push((row - 1, col))
        }
        if row != 0 && col != self.cols - 1 {
            res.push((row - 1, col + 1))
        }
        if col != 0 {
            res.push((row, col - 1))
        }
        if col != self.cols - 1 {
            res.push((row, col + 1))
        }
        if row != self.rows - 1 && col != 0 {
            res.push((row + 1, col - 1))
        }
        if row != self.rows - 1 {
            res.push((row + 1, col))
        }
        if row != self.rows - 1 && col != self.rows - 1 {
            res.push((row + 1, col + 1))
        }
        res
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum Cell {
    Dead,
    Alive,
}

impl Default for Cell {
    fn default() -> Self {
        Self::Dead
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(PartialEq, Eq)]
pub struct GameOfLife {
    grid: Grid<Cell>,
}

impl GameOfLife {
    pub fn from_grid(grid: Grid<Cell>) -> Self {
        GameOfLife { grid }
    }

    pub fn get_grid(&self) -> &Grid<Cell> {
        &self.grid
    }

    pub fn step(&mut self) {
        let mut grid = Grid::<Cell>::new(self.grid.rows, self.grid.cols);
        for i in 0..self.grid.rows {
            for j in 0..self.grid.cols {
                let alive = self
                    .grid
                    .neighbours(i, j)
                    .iter()
                    .filter(|cell| *self.grid.get(cell.0, cell.1) == Cell::Alive)
                    .count();

                if alive == 3 || *self.grid.get(i, j) == Cell::Alive && alive == 2 {
                    grid.set(Cell::Alive, i, j);
                } else {
                    grid.set(Cell::Dead, i, j);
                }
            }
        }
        self.grid = grid
    }
}
