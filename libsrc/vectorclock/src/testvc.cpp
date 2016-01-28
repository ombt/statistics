
#include <QDebug>
#include "hdr/vectorclock.h"

int
main(int argc, char **argv)
{
	VectorClock alice("alice", "dinner");
	alice.incrementClock();
	qDebug() << alice.toString();

	alice.incrementClock("ben");
	alice.incrementClock("ben");
	alice.incrementClock("ben");
	alice.incrementClock("ben");
	qDebug() << alice.toString();

	VectorClock ben("ben", "dinner");
	qDebug() << ben.toString();

	if (alice < ben)
		qDebug() << alice.toString() << "PREFIX OF" << ben.toString();
	else
		qDebug() << alice.toString() << "NOT A PREFIX OF" << ben.toString();

	if (ben < alice)
		qDebug() << ben.toString() << "PREFIX OF" << alice.toString();
	else
		qDebug() << ben.toString() << "NOT A PREFIX OF" << alice.toString();

	ben.incrementClock();
	qDebug() << ben.toString();

	if (alice < ben)
		qDebug() << alice.toString() << "PREFIX OF" << ben.toString();
	else if (ben < alice)
		qDebug() << ben.toString() << "PREFIX OF" << alice.toString();
	else
		qDebug() << ben.toString() << "NOT A PREFIX OF" << alice.toString();

        VectorClock dummy;
        qDebug() << "Dummy vector clock" << dummy.toString();

        dummy = alice.toString();
        qDebug() << "Alice vector clock" << alice.toString();
        qDebug() << "Dummy vector clock after assignment to alice" << dummy.toString();
        if (dummy == alice)
            qDebug() << "dummy and alice are the same.";
        else
            qDebug() << "ERROR: dummy and alice are NOT the same.";

        VectorClock dummy2(alice.toString());
        qDebug() << "Dummy2 vector clock after init using alice" << dummy2.toString();
        if (dummy2 == alice)
            qDebug() << "dummy2 and alice are the same.";
        else
            qDebug() << "ERROR: dummy2 and alice are NOT the same.";

	return(0);
}

