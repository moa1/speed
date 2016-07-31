// princess.c

/*
1 princess: if you play/drop her, you lose.
1 duchess: must play her if you have the king or prince.
1 king: exchange your card with another player.
2 princes: a player (may be self) must drop his/her card, and redraw another card.
2 abigails: you are protected in the next round.
2 barons: compare with another player, the one with the lower card loses.
2 priests: look at the card of a player.
5 watchwomen: if you guess the card of another player, he/she loses. may not guess watchwoman.

one card is put aside. each of the 4 players gets one card. then, for each player: the player draws a second card, and plays one of them. this continues until only one player remains, or no cards are left to draw. in the latter case the player with the highest card gets a heart, and must shuffle next round (so that the player next in clockwise order is the next first player). the first player to get 3 hearts wins.
*/

/* there are (/ (fak 16) (fak 5) (fak 2) (fak 2) (fak 2) (fak 2)) == 10897286400 (i.e. 11 * 10**9) different possible stack sortings.
if each card had a target and guess, there would be (2*4*16)**16 == 5192296858534827628530496329220096 == 5.192297e33 different games for each stack. */

#define _DEFAULT_SOURCE //for random()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// a card is represented as a uint4.
typedef enum card {
	princess = 15,
	duchess = 14,
	king = 13,
	prince1 = 12,
	prince2 = 11,
	abigail1 = 10,
	abigail2 = 9,
	baron1 = 8,
	baron2 = 7,
	priest1 = 6,
	priest2 = 5,
	watchwoman1 = 4,
	watchwoman2 = 3,
	watchwoman3 = 2,
	watchwoman4 = 1,
	watchwoman5 = 0,
} card_t;

const char* card2str(card_t card) {
	switch(card) {
		case princess: return "princess";
		case duchess: return "duchess";
		case king: return "king";
		case prince1: return "prince1";
		case prince2: return "prince2";
		case abigail1: return "abigail1";
		case abigail2: return "abigail2";
		case baron1: return "baron1";
		case baron2: return "baron2";
		case priest1: return "priest1";
		case priest2: return "priest2";
		case watchwoman1: return "watchwoman1";
		case watchwoman2: return "watchwoman2";
		case watchwoman3: return "watchwoman3";
		case watchwoman4: return "watchwoman4";
		case watchwoman5: return "watchwoman5";
		default: fprintf(stderr, "invalid card:%i\n",card); assert(0); //invalid card
	}
}

card_t standardize_card(card_t card) {
	card_t card2stdcard[16] = {
		watchwoman1,watchwoman1,watchwoman1,watchwoman1,watchwoman1,
		priest1, priest1,
		baron1, baron1,
		abigail1, abigail1,
		prince1, prince1,
		king,
		duchess,
		princess,
	};
	return card2stdcard[card];
}

typedef struct game {
	// the following must be backed up in enumerate_tries
	int num_stack; //number of cards on the stack
	int num_dump; //number of cards on the dump
	card_t players[4]; //each player has one card
	int lost; //a set bit's index names the player that has lost
	int protected; //a set bit's index names the protected player
	// the following do not have to be backed up in enumerate_tries
	card_t stack[16]; //the stack of cards, stack[0] is the last card
	int num_players; //number of total players (including those that lost)
	card_t dump[16]; //the stack of played cards, dump[num_dump-1] is most recent
} game_t;

/*copy source to dest*/
void copy_game_to_game(game_t* dest, game_t* source) {
	dest->num_stack = source->num_stack;
	for (int i=0; i<source->num_stack; i++) {
		dest->stack[i] = source->stack[i];
	}
	dest->num_players = source->num_players;
	for (int i=0; i<source->num_players; i++) {
		dest->players[i] = source->players[i];
	}
	dest->lost = source->lost;
	dest->protected = source->protected;
	dest->num_dump = source->num_dump;
	for (int i=0; i<source->num_dump; i++) {
		dest->dump[i] = source->dump[i];
	}
}

card_t pop_stack(game_t *game) {
	assert(game->num_stack > 0);
	game->num_stack--;
	return game->stack[game->num_stack];
}

card_t push_dump(game_t *game, card_t card) {
	assert(game->num_dump < 16);
	game->dump[game->num_dump] = card;
	game->num_dump++;
}

inline int player_lost(game_t *game, int player) {
	return (game->lost >> player) & 1;
}

inline void set_player_lost(game_t *game, int player) {
	push_dump(game, game->players[player]);
	game->lost |= 1<<player;
}

inline int player_protected(game_t *game, int player) {
	return (game->protected >> player) & 1;
}

inline void set_player_protected(game_t *game, int player) {
	game->protected |= 1<<player;
}

inline void unset_player_protected(game_t *game, int player) {
	game->protected &= ~(1<<player);
}

void print_game(game_t *game) {
	printf("PLAYERS:");
	for (int i=0; i<game->num_players; i++) {
		char* protected;
		if (player_lost(game,i)) {
			protected = "(lost)";
		} else if (player_protected(game, i)) {
			protected = "(protected)";
		} else {
			protected = "";
		}
		printf(" %s%s", card2str(game->players[i]), protected);
	}
	printf(" STACK:");
	for (int i=0; i<game->num_stack; i++) {
		printf(" %s", card2str(game->stack[i]));
	}
	printf(" DUMP:");
	for (int i=0; i<game->num_dump; i++) {
		printf(" %s", card2str(game->dump[i]));
	}
	printf("\n");
}

/*returns the card of the target, or -1 if the game continues, or -2 if the game is over.*/
inline int play(game_t* game, uint8_t player, card_t card, uint8_t target, card_t guess) {
	assert(!player_lost(game, target));
	unset_player_protected(game, player); //TODO: FIXME: move this to the end of this function, and execute it in the turn before PLAYER takes turn. The reason is that when electing players as possible targets, oneself (i.e. PLAYER) is not considered, because the un-setting of the protection bit here is done after electing.
	assert(!player_protected(game, target));
	push_dump(game, card);
	int ret=-1;
	switch(card){
		case watchwoman1: case watchwoman2: case watchwoman3: case watchwoman4: case watchwoman5:
			guess = standardize_card(guess); //check validity of variable guess
			if (guess != watchwoman1 && standardize_card(game->players[target]) == guess) {
				set_player_lost(game, target);
			}
			break;
		case priest1: case priest2:
			ret = game->players[target];
			break;
		case baron1: case baron2:
		{
			card_t card_player = standardize_card(game->players[player]);
		    card_t card_target = standardize_card(game->players[target]);
			if (card_player > card_target) {
				set_player_lost(game, target);
			} else if (card_player < card_target) {
				set_player_lost(game, player);
			}
		}
		break;
		case abigail1: case abigail2:
			set_player_protected(game, player);
			break;
		case prince1: case prince2:
		{
			assert(game->players[player] != duchess);
			if (game->players[target] == princess) {
				set_player_lost(game, target);
			} else {
				push_dump(game, game->players[target]);
				game->players[target] = pop_stack(game);
			}
		}
		break;
		case king:
			assert(game->players[player] != duchess);
			ret = game->players[player];
			card_t temp = game->players[player];
			game->players[player] = game->players[target];
			game->players[target] = temp;
			break;
		case duchess:
			break;
		case princess:
			set_player_lost(game, player);
			break;
		default: fprintf(stderr, "invalid card:%i\n",card); assert(0); //invalid card
	}
	return ret;
}

int game_over_p(game_t* game) {
	// check if game is over due to empty stack
	if (game->num_stack <= 1) {
		int winner=-1;
		int card=-1;
		for (int i=0; i<game->num_players; i++) {
			if ((!player_lost(game, i) && winner == -1) || (!player_lost(game, i) && game->players[i] > card)) {
				winner = i;
				card = game->players[i];
			}
		}
		card = standardize_card(card);
		for (int i=0; i<game->num_players; i++) {
			if (!player_lost(game, i) && standardize_card(game->players[i]) < card)
				set_player_lost(game, i);
		}
		return 1;
	}
	// check if game is over due to last player standing
	int num_players = game->num_players;
	for (int i=0; i<game->num_players; i++) {
		if (player_lost(game, i)) num_players--;
	}
	assert(num_players > 0);
	if (num_players == 1) {
		return 1;
	}
	return 0;
}

typedef struct trie {
	// properties during the trie
	int player; // the player that plays the trie
	card_t card; // the played card
	int target; // the target, or -1 if no target necessary
	card_t guess; // the guessed card, or -1 if guess is not necessary
} trie_t;

// print winners
void print_winners(game_t* game) {
	print_game(game);
	for (int i=0; i<game->num_players; i++) {
		if (!player_lost(game, i)) {
			printf("winner: %i ", i);
		}
	}
	printf("\n");
}

inline uint16_t unset_bit(uint16_t bits, int i) {
	bits &= ~(1<<i);
	return bits;
}

int calls = 0;

void enumerate_tries(game_t* game, int player, int turn, trie_t* tries) {
	card_t card1 = game->players[player];
	card_t card2 = pop_stack(game);

	unset_player_protected(game, player); //unset player protection, so that we can select ourself in this turn, if we had been protected.

	game_t backup;
	inline void backup_game(game_t* game) {
		backup.num_stack = game->num_stack;
		backup.num_dump = game->num_dump;
		backup.players[0] = game->players[0];
		backup.players[1] = game->players[1];
		backup.players[2] = game->players[2];
		backup.players[3] = game->players[3];
		backup.lost = game->lost;
		backup.protected = game->protected;
	}
	inline void restore_game(game_t* game) {
		game->num_stack = backup.num_stack;
		game->num_dump = backup.num_dump;
		game->players[0] = backup.players[0];
		game->players[1] = backup.players[1];
		game->players[2] = backup.players[2];
		game->players[3] = backup.players[3];
		game->lost = backup.lost;
		game->protected = backup.protected;
	}
//	copy_game_to_game(&backup, game);
	backup_game(game);
	
/*	printf("------- TURN:%i -------\n", turn);
	print_game(game);
*/	
	// compute possible targets
	int num_targets = 0;
	int targets[game->num_players];
	for (int i=0; i<game->num_players; i++) {
		if (!player_lost(game, i) && !player_protected(game, i)) {
			targets[num_targets] = i;
			num_targets++;
		}
	}
	// compute possible guesses
	uint16_t guesses = (uint16_t)-1;
	guesses = unset_bit(guesses, card1);
	guesses = unset_bit(guesses, card2);
	for (int i=0; i<game->num_dump; i++) {
		guesses = unset_bit(guesses, game->dump[i]);
	}
	{
		int last_guess=-1;
		for (int i=0; i<16; i++) {
			if ((guesses & (1<<i)) != 0) {
				if (last_guess == -1) {
					last_guess = standardize_card(i);
				} else if (last_guess == standardize_card(i)) {
					guesses = unset_bit(guesses, i);
				} else { //last_guess != standardize_card(i)
					last_guess = standardize_card(i);
				}
			}
		}
	}

/*	printf("guess:");
	for (card_t i_guess=0; i_guess<16; i_guess++) {
		if ((guesses & (1<<i_guess)) != 0) {
			printf(" %s",card2str(i_guess));
		}
	}
	printf("\n");
*/
	
	void play_trie2(game_t *game, int player, card_t card, int target, card_t guess) { 
//		if ((calls%1000)==0) fprintf(stderr, "calls:%i\n",calls);
		calls++;
		
//		printf("PLAYER: %i CARD1: %s CARD2: %s CARD: %s TARGET: %i GUESS: %s\n", player, card2str(card1), card2str(card2), card2str(card), target, (guess>=0 && guess<=15)?card2str(guess):"none");

		tries[turn].player = player;
		tries[turn].card = card;
		tries[turn].target = target;
		tries[turn].guess = guess;
		play(game, player, card, target, guess);
		if (game_over_p(game)) {
/*			
			printf("winner ");
			//print_winners(game);
			for (int i=0; i<turn+1; i++) {
				card_t guess = tries[i].guess;
				printf("turn:%i player:%i card:%s target:%i guess:%s, ",i,tries[i].player,card2str(tries[i].card),tries[i].target,(guess>=0 && guess<=15)?card2str(guess):"none");
			}
			printf("\n");
*/
		} else {
			int next_player = player;
			while (1) {
				next_player = (next_player+1)%game->num_players;
				if (!player_lost(game, next_player))
					break;
			}
			enumerate_tries(game, next_player, turn+1, tries);
		}
//		copy_game_to_game(game, &backup);
		restore_game(game); //restore game at beginning of this turn
/*		printf("--- BACKTRACK TURN:%i ---\n", turn);
		print_game(game);
*/
	}
		
	void play_trie(card_t card, card_t keep) {
		game->players[player] = keep;
		switch(card) {
			case watchwoman1: case watchwoman2: case watchwoman3: case watchwoman4: case watchwoman5:
				for (int i_target=0; i_target<num_targets; i_target++) {
					for (card_t i_guess=5; i_guess<16; i_guess++) { //don't guess watchwoman: start at 5
						if ((guesses & (1<<i_guess)) != 0) {
							play_trie2(game, player, card, targets[i_target], i_guess);
						}
					}
				}
				break;
			case priest1: case priest2:
				play_trie2(game, player, card, -1, -1);
				break;
			case baron1: case baron2:
				for (int i_target=0; i_target<num_targets; i_target++) {
					play_trie2(game, player, card, targets[i_target], -1);
				}
				break;
			case abigail1: case abigail2:
				play_trie2(game, player, card, -1, -1);
				break;
			case prince1: case prince2:
				for (int i_target=0; i_target<num_targets; i_target++) {
					play_trie2(game, player, card, targets[i_target], -1);
				}
				break;
			case king:
				for (int i_target=0; i_target<num_targets; i_target++) {
					play_trie2(game, player, card, targets[i_target], -1);
				}
				break;
			case duchess:
				play_trie2(game, player, card, -1, -1);
				break;
			case princess:
				play_trie2(game, player, card, -1, -1);
				break;
			default: fprintf(stderr, "invalid card:%i\n",card); assert(0); //invalid card
		}
	}

	// check if player is forced to play a card
	if (standardize_card(card1) == standardize_card(card2)) {
		play_trie(card1, card2); // only need one trie since card1 == card2
	} else if ((card1 != duchess && card2 != duchess) && (card1 != princess && card2 != princess)) {
		play_trie(card1, card2);
		play_trie(card2, card1);
	} else { // one of the cards is duchess or princess
		if (card2 == duchess || card2 == princess) {
			card_t temp = card1;
			card1 = card2;
			card2 = temp;
		}
		if (card1 == duchess && (card2 == king || standardize_card(card2) == prince1)) {
			play_trie(card1, card2);
		} else if (card1 == princess) {
			play_trie(card2, card1);
		} else {
			play_trie(card1, card2);
			play_trie(card2, card1);
		}
	}
}

void init_first_game(game_t* game, int num_players) {
	game->num_stack = 16;
	for (int i=0; i<16; i++) {
		game->stack[i] = i;
	}
	game->num_players = num_players;
	for (int i=0; i<game->num_players; i++) {
		game->players[i] = pop_stack(game);
	}
	game->lost = 0;
	game->protected = 0;
	game->num_dump = 0;
}

void init_random_game(game_t* game, int num_players, int num_stack) {
	game->num_stack = num_stack;
	for (int i=0; i<game->num_stack; i++) {
		game->stack[i] = 15-i;
	}
	// shuffle stack
	for (int i=game->num_stack-1; i>=1; i--) {
		int r=random()%i;
		card_t temp = game->stack[i];
		game->stack[i] = game->stack[r];
		game->stack[r] = temp;
	}
	game->num_players = num_players;
	for (int i=0; i<game->num_players; i++) {
		game->players[i] = pop_stack(game);
	}
	game->lost = 0;
	game->protected = 0;
	game->num_dump = 0;
}

int main(int argc, char** argv) {
	game_t game_s;
	game_t *game = &game_s;

	init_random_game(game, 4, atoi(argv[1]));
	print_game(game);
	
	trie_t tries[16];
	enumerate_tries(game, 0, 0, tries);

	fprintf(stderr, "calls:%i\n", calls);
}
