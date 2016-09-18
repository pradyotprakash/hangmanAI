import operator
from collections import Counter

words = {}
frequencies = {}
n = 0
maxLen = -1

for c in 'abcdefghijklmnopqrstuvwxyz':
	frequencies[c] = 0.0

with open('/usr/share/dict/words') as f:
	for x in f:
		y = x.lower().strip()
		
		if y.isalpha():
			l = len(y)
			if l > maxLen:
				maxLen = l

			for c in y:
				n += 1
				frequencies[c] += 1
			
			if l in words:
				words[l].append(y)
			else:
				words[l] = [y]

frequencies = {k: frequencies[k]/n for k in frequencies}

def printBlanks(blanks, missList):
	for i in xrange(len(blanks)):
		if blanks[i] is None:
			print '_ ',
		else:
			print '%s '%blanks[i],

	print 'missed: ',
	for c in missList:
		print '%s,'%c,
	print
	print


def getMaxFreqChar(l, hl, ml):
	z = Counter(''.join(l))
	for c in hl + ml:
		del z[c]
	return max(z.iteritems(), key=operator.itemgetter(1))[0]

correct = 0
false = 0
with open('words_50000.txt', 'r') as f:
# with open('test.txt', 'r') as f:
	for givenWord in f:
		try:
			print correct + false
			missList = []
			hitList = []
			lenRemaining = len(givenWord)
			blanks = [None]*lenRemaining

			# printBlanks(blanks, missList)

			# approximate this better
			if lenRemaining > maxLen:
				false += 1
				continue

			relevantWords = words[lenRemaining]
			while len(missList) < 6 and lenRemaining > 0:

				def func(x):
					for c in hitList:
						if c not in x: return False
					for c in missList:
						if c in x: return False
					return True

				relevantWords = filter(func, relevantWords)
				c = getMaxFreqChar(relevantWords, hitList, missList)
				
				# print 'guess: %s'%c

				if c in givenWord:
					hitList.append(c)
					for i in xrange(len(givenWord)):
						if givenWord[i] == c:
							blanks[i] = c
							lenRemaining -= 1
				else:
					missList.append(c)
				# printBlanks(blanks, missList)

			if None in blanks:
				false += 1
			else:
				print ''.join(blanks), givenWord
				if ''.join(blanks) == givenWord:
					correct += 1
				else:
					false += 1
		except Exception as e:
			print givenWord

print correct, false, correct/(correct+false)