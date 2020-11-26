from random import seed
from random import choice
import rook


class GameBoard:

    def __init__(self):
        # generate the board square colors
        self._board= [[]]
        self._possible_values = [ord('r'), ord('b'), ord('g'), ord('y')]
        self.BOARD_SIZE = 18
        self.NUM_TRIES = 21
        
        seed(1) # this needs to be random every time
        
        for i in range(self.BOARD_SIZE):
            values = []
            for _ in range(self.BOARD_SIZE):
                values.append(choice(self._possible_values))
            self._board.insert(i, values)
    
    def is_valid_position(self, i, j):
        return (i>=0 and j>=0) and (i<self.BOARD_SIZE and j<self.BOARD_SIZE)

    def should_color_square(self, current_color, i, j):
        # we color the squre in the following cases:
        # 1. it is the current color
        # 2. it is a left or right neighbor with current color
        # 3. it is a above or below neighbor with current color
        # as we move from "right to left" and "top to bottom" we only
        # really care about not visited positions: to the left, and above
        if self._board[i][j] == current_color and  \
           (self.is_valid_position(i,j-1) and  self._board[i][j-1] == current_color) or \
           (self.is_valid_position(i+1,j) and  self._board[i+1][j] == current_color):
            return True
        return False


    def process_play(self, color_chosen):
        #print (color_chosen)
        # for now we start at [0][0]
        current_color = self._board[0][0]
        self._board[0][0] = color_chosen # always color this
        for i in range(len(self._board)):
            for j in range(len(self._board[i])):
                if self.should_color_square(current_color, i, j):
                    self._board[i][j] = color_chosen

    def did_i_win(self, color_chosen):
        for i in range(len(self._board)):
            for j in range(len(self._board[i])):
                if self._board[i][j] != color_chosen:
                    return False
        return True

    def get_user_input(self):
        val = None
        num_tries = self.NUM_TRIES
        while val not in ['r', 'b', 'g', 'y']:
            val = input("Enter a color (r:b:g:y): ")
            print ("bad input. try again")

        while num_tries > 0:
            self.process_play(ord(val))
            if self.did_i_win(ord(val)):
                print ("you win!!")
                return
            num_tries = num_tries-1

        print("game ended")

    def print_board(self):
        print (self._board)

def main():
    B = GameBoard()
    B.get_user_input()
    B.print_board()


 

if __name__ == "__main__":
    rook.start(token='bf10e09b66618e23007bff44236e78842bce6beb86703d9d66a4845cd28fe2af', labels={"env":"dev"})
    main()