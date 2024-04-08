greek_letters = ('alpha', 'beta', 'gamma', 'delta', 'epsilon', 'zeta', 'eta', 
                 'theta', 'iota', 'kappa')

roman_numerals = ('I', 'II', 'III', 'IV', 'V', 'VI', 'VII', 'VIII', 'IX', 'X', 'XI', 
                  'XII', 'XIII', 'XIV', 'XV', 'XVI', 'XVII', 'XVIII', 'XIX', 'XX', 'XXI', 
                  'XXII', 'XXIII', 'XXIV', 'XXV', 'XXVI', 'XXVII', 'XXVIII', 'XXIX', 'XXX')

a, b = int(input()), int(input())

for i in range(a):
    print(*[greek_letters[i] + '_' + roman_numerals[j] for j in range(b)])
