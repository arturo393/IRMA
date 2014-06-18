#ifndef MEMORYTRAINER_H
#define MEMORYTRAINER_H

#include <QThread>
class MemoryTrainer :public QThread{
    Q_OBJECT
public:
    MemoryTrainer();
    MemoryTrainer(const MemoryTrainer &);
    MemoryTrainer(int id);
    int id;
protected:
    void run();
};

#endif // MEMORYTRAINER_H
