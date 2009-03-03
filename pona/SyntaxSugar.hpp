/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

inline static NODE CHOICE(NODE choice0, NODE choice1) {
	return OR(choice0, choice1);
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2) {
	return OR(choice0, OR(choice1, choice2));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3) {
	return OR(choice0, OR(choice1, OR(choice2, choice3)));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, choice4))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, choice5)))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, choice6))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, choice7)))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, choice8))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, choice9)))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, choice10))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, choice11)))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, choice12))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, choice13)))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, choice14))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, choice15)))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, choice16))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, choice17)))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, choice18))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, choice19)))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, choice20))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, choice21)))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, choice22))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, choice23)))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, choice24))))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24, NODE choice25) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, OR(choice24, choice25)))))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24, NODE choice25, NODE choice26) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, OR(choice24, OR(choice25, choice26))))))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24, NODE choice25, NODE choice26, NODE choice27) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, OR(choice24, OR(choice25, OR(choice26, choice27)))))))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24, NODE choice25, NODE choice26, NODE choice27, NODE choice28) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, OR(choice24, OR(choice25, OR(choice26, OR(choice27, choice28))))))))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24, NODE choice25, NODE choice26, NODE choice27, NODE choice28, NODE choice29) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, OR(choice24, OR(choice25, OR(choice26, OR(choice27, OR(choice28, choice29)))))))))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24, NODE choice25, NODE choice26, NODE choice27, NODE choice28, NODE choice29, NODE choice30) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, OR(choice24, OR(choice25, OR(choice26, OR(choice27, OR(choice28, OR(choice29, choice30))))))))))))))))))))))))))))));
}
inline static NODE CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15, NODE choice16, NODE choice17, NODE choice18, NODE choice19, NODE choice20, NODE choice21, NODE choice22, NODE choice23, NODE choice24, NODE choice25, NODE choice26, NODE choice27, NODE choice28, NODE choice29, NODE choice30, NODE choice31) {
	return OR(choice0, OR(choice1, OR(choice2, OR(choice3, OR(choice4, OR(choice5, OR(choice6, OR(choice7, OR(choice8, OR(choice9, OR(choice10, OR(choice11, OR(choice12, OR(choice13, OR(choice14, OR(choice15, OR(choice16, OR(choice17, OR(choice18, OR(choice19, OR(choice20, OR(choice21, OR(choice22, OR(choice23, OR(choice24, OR(choice25, OR(choice26, OR(choice27, OR(choice28, OR(choice29, OR(choice30, choice31)))))))))))))))))))))))))))))));
}

inline static NODE GLUE(NODE node0, NODE node1) {
	node0->next_ = node1;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2) {
	node0->next_ = node1;
	node1->next_ = node2;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24, NODE node25) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	node24->next_ = node25;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24, NODE node25, NODE node26) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	node24->next_ = node25;
	node25->next_ = node26;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24, NODE node25, NODE node26, NODE node27) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	node24->next_ = node25;
	node25->next_ = node26;
	node26->next_ = node27;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24, NODE node25, NODE node26, NODE node27, NODE node28) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	node24->next_ = node25;
	node25->next_ = node26;
	node26->next_ = node27;
	node27->next_ = node28;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24, NODE node25, NODE node26, NODE node27, NODE node28, NODE node29) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	node24->next_ = node25;
	node25->next_ = node26;
	node26->next_ = node27;
	node27->next_ = node28;
	node28->next_ = node29;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24, NODE node25, NODE node26, NODE node27, NODE node28, NODE node29, NODE node30) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	node24->next_ = node25;
	node25->next_ = node26;
	node26->next_ = node27;
	node27->next_ = node28;
	node28->next_ = node29;
	node29->next_ = node30;
	return node0;
}
inline static NODE GLUE(NODE node0, NODE node1, NODE node2, NODE node3, NODE node4, NODE node5, NODE node6, NODE node7, NODE node8, NODE node9, NODE node10, NODE node11, NODE node12, NODE node13, NODE node14, NODE node15, NODE node16, NODE node17, NODE node18, NODE node19, NODE node20, NODE node21, NODE node22, NODE node23, NODE node24, NODE node25, NODE node26, NODE node27, NODE node28, NODE node29, NODE node30, NODE node31) {
	node0->next_ = node1;
	node1->next_ = node2;
	node2->next_ = node3;
	node3->next_ = node4;
	node4->next_ = node5;
	node5->next_ = node6;
	node6->next_ = node7;
	node7->next_ = node8;
	node8->next_ = node9;
	node9->next_ = node10;
	node10->next_ = node11;
	node11->next_ = node12;
	node12->next_ = node13;
	node13->next_ = node14;
	node14->next_ = node15;
	node15->next_ = node16;
	node16->next_ = node17;
	node17->next_ = node18;
	node18->next_ = node19;
	node19->next_ = node20;
	node20->next_ = node21;
	node21->next_ = node22;
	node22->next_ = node23;
	node23->next_ = node24;
	node24->next_ = node25;
	node25->next_ = node26;
	node26->next_ = node27;
	node27->next_ = node28;
	node28->next_ = node29;
	node29->next_ = node30;
	node30->next_ = node31;
	return node0;
}
