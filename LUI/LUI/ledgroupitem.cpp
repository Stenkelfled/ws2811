#include <QBrush>
#include <QObject>
#include <QPen>
#include <QtDebug>

#include "ledgroupitem.h"
#include <ledscene.h>
#include <protocoll.h>

LedGroupItem::LedGroupItem(qint16 id, QGraphicsItem *parent):
    LuiItem(id, parent),
    leds(new QList<QList<LedItem*>*>),
    my_sequences(new QList<SequenceItem*>),
    my_alignment(LedGroupItem::horizontal),
    my_name(QString::number(id))
{
    setPen(Qt::SolidLine);
    QColor c(settings::ledgroupitem::color);
    setBrush(QBrush(c.toHsv()));
    c = QColor(settings::leditem::color);
    this->my_color = c.toHsv();
    setAcceptDrops(true);

    //actions for context menu
    this->horAlignAct = new QAction(tr("&horizontal anordnen"), this);
    this->horAlignAct->setCheckable(true);
    connect(this->horAlignAct, SIGNAL(triggered()), this, SLOT(horAlign()));

    this->verAlignAct = new QAction(tr("&vertikal anordnen"), this);
    this->verAlignAct->setCheckable(true);
    connect(this->verAlignAct, SIGNAL(triggered()), this, SLOT(verAlign()));

    this->alignActGroup = new QActionGroup(this);
    this->alignActGroup->addAction(this->horAlignAct);
    this->alignActGroup->addAction(this->verAlignAct);
    this->horAlignAct->setChecked(true);

    this->nameAct = new QAction(tr("Name..."), this);
    connect(this->nameAct, SIGNAL(triggered()), this, SLOT(getName()));

    SequenceItem* seq = new SequenceItem();
    this->my_sequences->append(seq);
}

LedGroupItem::~LedGroupItem()
{
    foreach(QList<LedItem*>* row, *(this->leds)){
        delete row;
    }
    delete this->leds;
    foreach(SequenceItem* seq, *(this->my_sequences)){
        delete seq;
    }
    delete this->my_sequences;
}

void LedGroupItem::addLed(LedItem *led, qint16 row)
{
    if( (this->leds->size() == 0) || (row < 0) ){
        QList<LedItem*>* new_row = new QList<LedItem*>;
        leds->append(new_row);
        row = 0;
    }
    if( row >= this->leds->size()){
        QList<LedItem*>* new_row = new QList<LedItem*>;
        leds->append(new_row);
        row = this->leds->size() - 1;
    }
    if(led->parentItem() != NULL){
        LedGroupItem *group = qgraphicsitem_cast<LedGroupItem *>(led->parentItem());
        group->removeLed(led);
    }
    this->leds->at(row)->append(led);
    led->setParentItem(this);
    refreshArea();
}

quint16 LedGroupItem::rows() const
{
    return (quint16 const)this->leds->size();
}

void LedGroupItem::removeLed(LedItem *led)
{
    led->setParentItem(NULL);
    disconnect(led, 0, this, 0);
    foreach(QList<LedItem*>* row, *(this->leds)){
        int idx = row->indexOf(led);
        if(idx != -1){
            row->removeAt(idx);
            if(row->isEmpty()){
                delete row;
                this->leds->removeAll(row);
            }
            break;
        }
    }
    refreshArea();
    if(this->leds->isEmpty()){
        emit groupEmpty(this);
    }
}

QPoint LedGroupItem::indexOf(LedItem *led)
{
    QList<LedItem*> *row;
    int row_idx, col_idx;
    for(row_idx=0; row_idx<this->leds->size(); row_idx++){
        row = this->leds->at(row_idx);
        col_idx = row->indexOf(led);
        if(col_idx != -1){
            return QPoint(row_idx, col_idx);
        }
    }
    return QPoint(-1,-1);
}

void LedGroupItem::makeEmpty()
{
    foreach(QList<LedItem*>* row, *(this->leds)){
        foreach( LedItem* led, *(row)){
            led->setParentItem(NULL);
            disconnect(led, 0, this, 0);
        }
        delete row;
    }
    this->leds->clear();
    emit groupEmpty(this);
}

QColor LedGroupItem::color()
{
    return this->my_color;
}

void LedGroupItem::setColor(QColor color)
{
    if(color != this->my_color){
        this->my_color = color;
        foreach(QList<LedItem*>* row, *(this->leds)){
            foreach(LedItem *led, *(row)){
                led->setColor(color, true);
            }
        }
    }
}

QString LedGroupItem::name()
{
    return this->my_name;
}

void LedGroupItem::setName(const QString name)
{
    if( (!name.isEmpty()) && (name != this->my_name) ){
        this->my_name = name;
        if(isSelected()){
            emit nameChanged(name, this);
        }
    }
}

QList<SequenceItem *> *LedGroupItem::sequences()
{
    return this->my_sequences;
}

/**
 * @brief GroupItem::getUsbCmd
 * Generates the USB-command for this group. First generates a list of all leds in this group, then defines the sequence for this group.
 * @return The USB-command for this group
 */
QByteArray LedGroupItem::getUsbCmd()
{

    QByteArray cmd = QByteArray();

    //first: define leds in group
    #define STEP_NO_SER             (1<<10)
    #define STEP_WAIT_FOR_VALUE     (2<<10)
    int first_led;
    int current_led;
    int step = STEP_NO_SER;
    foreach(QList<LedItem*>* row, *(this->leds)){
        foreach(LedItem* led, *(row)){
            switch(step){
                case STEP_NO_SER:
                    //start a new series
                    first_led = led->id();
                    step = STEP_WAIT_FOR_VALUE;
                    break;
                case STEP_WAIT_FOR_VALUE:
                    current_led = led->id();
                    step = current_led - first_led;
                    break;
                default:
                    if( (led->id()-current_led) != step ){
                        //step does not fit any more --> end series
                        cmd.append(PR_GRP_LED_SER);
                        cmd.append(first_led);
                        cmd.append(current_led);
                        cmd.append(step);
                        first_led = led->id();
                        step = STEP_WAIT_FOR_VALUE;
                    } else {
                        current_led = led->id();
                    }
                    break;
            }
        }
    }
    switch(step){
        case STEP_NO_SER:
            //shold not happen, because then the group would be empty...
            break;
        case STEP_WAIT_FOR_VALUE:
            //there is one led not assigned to a group -> append as single led
            cmd.append(PR_GRP_LED_ADD);
            cmd.append(first_led);
            break;
        default:
            //unfinished series -> finish it
            cmd.append(PR_GRP_LED_SER);
            cmd.append(first_led);
            cmd.append(current_led);
            cmd.append(step);
            break;
    }

    //second: define sequences
    //TODO


#if 0
    //for debugging: only print all leds-ids in the group's order
    foreach(QList<LedItem*>* row, *(this->leds)){
        foreach(LedItem* led, *(row)){
            cmd.append(led->id());
        }
    }
#endif
    return cmd;
}

void LedGroupItem::refreshArea(bool item_moving)
{
    //qDebug() << "refresh" << item_moving;
    qreal x=0, y=0;
    foreach(QList<LedItem*>* row, *(this->leds)){
        x = 0;
        foreach(LedItem* led, *(row)){
            if(this->my_alignment == LedGroupItem::horizontal){
                led->setPos(x,y);
            } else {
                led->setPos(y,x);
            }
            //qDebug() << "led" << led->id() << "pos" << led->pos();
            x += settings::leditem::width + settings::leditem::spacing;
        }
        y += settings::leditem::height + settings::leditem::spacing;
    }

    QRectF r = this->childrenBoundingRect();
    int extra_space = 0;
    if(item_moving){
        extra_space = settings::ledgroupitem::extra_space;
    }
    r.setWidth(r.width() + 2*(settings::ledgroupitem::border + extra_space));
    r.setHeight(r.height() + 2*(settings::ledgroupitem::border + extra_space));
    r.translate(-(settings::ledgroupitem::border + extra_space), -(settings::ledgroupitem::border + extra_space));
    setRect(r);
}

void LedGroupItem::horAlign()
{
    this->horAlignAct->setChecked(true);
    this->my_alignment = LedGroupItem::horizontal;
    refreshArea();
}

void LedGroupItem::verAlign()
{
    this->verAlignAct->setChecked(true);
    this->my_alignment = LedGroupItem::vertical;
    refreshArea();
}

void LedGroupItem::getName()
{
    bool ok;
    QString name = QInputDialog::getText(NULL, tr("Name..."),
                                         tr("Name:"), QLineEdit::Normal,
                                         this->my_name, &ok);
    if (ok && !name.isEmpty())
        setName(name);
}

/*void GroupItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "Mouse press on group";
    //refreshArea(false);
#if 0
    QString leds;
    foreach(LedItem* led, *(this->grp)){
        leds += QString::number(led->id());
    }
    qDebug() << "leds:" << leds;
#endif
    LuiItem::mousePressEvent(event);
}*/

void LedGroupItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(this->nameAct);
    menu.addSeparator();
    menu.addAction(this->horAlignAct);
    menu.addAction(this->verAlignAct);
    menu.exec(event->screenPos());
}

void LedGroupItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    //qDebug() << "group drag enter";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dragEnterEvent(event);
    }
    event->accept();
    if(indexOf(led) == QPoint(-1,-1)){
        addLed(led);
    }
    refreshArea(true);
}

void LedGroupItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    //qDebug() << "group drag move";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dragMoveEvent(event);
        return;
    }
    event->accept();
    int new_col_idx = int(event->pos().x()+settings::leditem::spacing/2)/(settings::leditem::width+settings::leditem::spacing);
    int new_row_idx = int(event->pos().y()+settings::leditem::spacing/2)/(settings::leditem::height+settings::leditem::spacing);
    if(event->pos().y() < 0){
        new_row_idx--;
    }
    if(this->my_alignment == LedGroupItem::vertical){
        int tmp = new_col_idx;
        new_col_idx = new_row_idx;
        new_row_idx = tmp;
    }
    //qDebug() << "idx" << new_col_idx << "," << new_row_idx;
    QList<LedItem*>* row;
    int led_idx;
    int row_idx;
    for(row_idx=0; row_idx<this->leds->size(); row_idx++){
        row = this->leds->at(row_idx);
        led_idx = row->indexOf(led);
        if(led_idx != -1){
            row->removeAt(led_idx);
            break;
        }
    }
    QList<LedItem*>* new_row;
    if( (new_row_idx < 0) || (new_row_idx >= this->leds->size()) ){
        new_row = new QList<LedItem*>;
        if(new_row_idx < 0){
            this->leds->prepend(new_row);
        } else {
            this->leds->append(new_row);
        }
    } else {
        if(row_idx != new_row_idx){
            new_row = this->leds->at(new_row_idx);
        } else {
            new_row = row;
        }
    }
    if(new_col_idx >= new_row->size()){
        new_row->append(led);
    } else {
        new_row->insert(new_col_idx, led);
    }
    if(row->isEmpty()){
        this->leds->removeAll(row);
        delete row;
        row = NULL;
        //qDebug() << "row deleted" << row_idx;
    }
    refreshArea(true);
}

void LedGroupItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    //qDebug() << "group leave";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dragLeaveEvent(event);
        return;
    }
    removeLed(led);
    led->update();
    event->accept();
    refreshArea(false);
}

void LedGroupItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    //qDebug() << "Group drop";
    LedItem* led = LedItem::unpackDragData(event->mimeData());
    if(led == NULL){
        LuiItem::dropEvent(event);
        return;
    }
    event->accept();
    //qDebug() << "dropped Led" << led_id << ok;
    refreshArea(false);
}

QDataStream &operator<<(QDataStream &stream, const LedGroupItem &group){
    stream << (LuiItem&)(group);
    stream << group.my_color;
    stream << group.my_alignment;
    stream << group.my_name;
    stream << group.pos();
    stream << (quint16)(group.leds->size());
    foreach(QList<LedItem*>* row, *(group.leds)){
        stream << (quint16)(row->size());
        foreach(LedItem* led, *row){
            stream << *led;
        }
    }
    return stream;
}

QDataStream &operator>>(QDataStream &stream, LedGroupItem &group){
    stream >> (LuiItem&)(group);

    QColor group_color;
    stream >> group_color;
    group.setColor(group_color);

    stream >> group.my_alignment;
    if(group.my_alignment == LedGroupItem::horizontal){
        group.horAlignAct->setChecked(true);
    } else {
        group.verAlignAct->setChecked(true);
    }

    stream >> group.my_name;

    QPointF group_pos;
    stream >> group_pos;
    group.setPos(group_pos);

    quint16 row_count;
    stream >> row_count;
    LedItem* led;
    quint16 led_count;
    for(quint16 row_num=0; row_num<row_count; row_num++){
        stream >> led_count;
        for(quint16 led_num=0; led_num<led_count; led_num++){
            led = ((LedScene*)(group.scene()))->newLed(0);
            stream >> *led;
            group.addLed(led, row_num);
        }
    }
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const LedGroupItem::groupAlignment &alignment){
    stream << (qint32&)alignment;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, LedGroupItem::groupAlignment &alignment){
    stream >> (qint32&)alignment;
    return stream;
}























