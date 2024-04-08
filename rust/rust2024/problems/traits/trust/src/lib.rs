#![forbid(unsafe_code)]

#[derive(Default, Clone, Copy, PartialEq, Eq)]
pub enum Strategy {
    #[default]
    Cooperate,
    Cheat,
}

pub trait Agent {
    fn get_move(&self, round_number: usize) -> Strategy;

    fn set_opponent_move(&mut self, opponent_move: Strategy);
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum RoundOutcome {
    BothCooperated,
    LeftCheated,
    RightCheated,
    BothCheated,
}

pub struct Game {
    left: Box<dyn Agent>,
    right: Box<dyn Agent>,
    left_score: i32,
    right_score: i32,
    round_number: usize,
}

impl Game {
    pub fn new(left: Box<dyn Agent>, right: Box<dyn Agent>) -> Self {
        Self {
            left,
            right,
            left_score: 0,
            right_score: 0,
            round_number: 0,
        }
    }

    pub fn left_score(&self) -> i32 {
        self.left_score
    }

    pub fn right_score(&self) -> i32 {
        self.right_score
    }

    pub fn play_round(&mut self) -> RoundOutcome {
        let left_move = self.left.get_move(self.round_number);
        let right_move = self.right.get_move(self.round_number);

        let outcome = match (left_move, right_move) {
            (Strategy::Cheat, Strategy::Cheat) => RoundOutcome::BothCheated,
            (Strategy::Cooperate, Strategy::Cheat) => {
                self.left_score -= 1;
                self.right_score += 3;
                RoundOutcome::RightCheated
            }
            (Strategy::Cheat, Strategy::Cooperate) => {
                self.left_score += 3;
                self.right_score -= 1;
                RoundOutcome::LeftCheated
            }
            (Strategy::Cooperate, Strategy::Cooperate) => {
                self.left_score += 2;
                self.right_score += 2;
                RoundOutcome::BothCooperated
            }
        };

        self.left.set_opponent_move(right_move);
        self.right.set_opponent_move(left_move);

        self.round_number += 1;

        outcome
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Default)]
pub struct CheatingAgent {}

impl Agent for CheatingAgent {
    fn get_move(&self, _: usize) -> Strategy {
        Strategy::Cheat
    }

    fn set_opponent_move(&mut self, _: Strategy) {}
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Default)]
pub struct CooperatingAgent {}

impl Agent for CooperatingAgent {
    fn get_move(&self, _: usize) -> Strategy {
        Strategy::Cooperate
    }

    fn set_opponent_move(&mut self, _: Strategy) {}
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Default)]
pub struct GrudgerAgent {
    should_cheat: bool,
}

impl Agent for GrudgerAgent {
    fn get_move(&self, _: usize) -> Strategy {
        if self.should_cheat {
            Strategy::Cheat
        } else {
            Strategy::Cooperate
        }
    }

    fn set_opponent_move(&mut self, opponent_move: Strategy) {
        self.should_cheat |= opponent_move == Strategy::Cheat;
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Default)]
pub struct CopycatAgent {
    opponent_prev_move: Strategy,
}

impl Agent for CopycatAgent {
    fn get_move(&self, round_number: usize) -> Strategy {
        match round_number {
            0 => Strategy::Cooperate,
            _ => self.opponent_prev_move,
        }
    }

    fn set_opponent_move(&mut self, opponent_move: Strategy) {
        self.opponent_prev_move = opponent_move;
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Default)]
pub struct DetectiveAgent {
    opponent_prev_move: Strategy,
    opponent_cheated: bool,
}

impl Agent for DetectiveAgent {
    fn get_move(&self, round_number: usize) -> Strategy {
        match round_number {
            0 => Strategy::Cooperate,
            1 => Strategy::Cheat,
            2 => Strategy::Cooperate,
            3 => Strategy::Cooperate,
            _ => {
                if !self.opponent_cheated {
                    Strategy::Cheat
                } else {
                    self.opponent_prev_move
                }
            }
        }
    }

    fn set_opponent_move(&mut self, opponent_move: Strategy) {
        self.opponent_cheated |= opponent_move == Strategy::Cheat;
        self.opponent_prev_move = opponent_move;
    }
}
