#ifndef LIOITERATORADAPTER_HPP
#define LIOITERATORADAPTER_HPP
#include <QString>
#include <QSet>

class LIOIteratorAdapter
{
private:
    const QSet<QString>& container;
    int index_current  = 0;
public:
    LIOIteratorAdapter(const QSet<QString>& container):container(container){}
    ~LIOIteratorAdapter(){}
public:
    bool have_next(){
        bool have = false;
        if(!container.isEmpty()){
            if(index_current < container.size()){
                have = true;
            }
        }
        return have;
    }

    QString next(){
        QString str = "";
        if(have_next()){
            str = container.
        }
        index_current++;
        return str;
    }



};

#endif // LIOITERATORADAPTER_HPP
