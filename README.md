# Calculate-Word-Distance
C++ program that calculates the "distance" between two words of a given length. Distance is calculated using a Thue system-like grammar rule. For example the distance between "CAT" and "BAT" is 1, because one letter is changed (C to B). The distance between "CAT" and "BAR" is 2 (CAT -> CAR -> BAR).

This program parses all the words in the Unix dictionary (located at /usr/dict/words in most Unix systems) and finds all the words of length N. It builds an array from those words, and then converts that array into a graph, stored internally as an adjacency list. In the graph, each word is a node, and each node is aware of all its neighbors.

I used a modified Breadth First Search algorithm to find the shortest path between two words in the graph. I also wrote it so that the path can be printed out.
