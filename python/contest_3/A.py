greek_letters = ('alpha', 'beta', 'gamma', 'delta', 'epsilon', 'zeta', 'eta', 
                 'theta', 'iota', 'kappa')

roman_numerals = ('I', 'II', 'III', 'IV', 'V', 'VI', 'VII', 'VIII', 'IX', 'X', 'XI', 
                  'XII', 'XIII', 'XIV', 'XV', 'XVI', 'XVII', 'XVIII', 'XIX', 'XX', 'XXI', 
                  'XXII', 'XXIII', 'XXIV', 'XXV', 'XXVI', 'XXVII', 'XXVIII', 'XXIX', 'XXX')

s = input()

print(greek_letters.index(s.split('_')[0]) + 1, roman_numerals.index(s.split('_')[1]) + 1)
