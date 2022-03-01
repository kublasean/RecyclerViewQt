#ifndef CHANNELITEMMODEL_H
#define CHANNELITEMMODEL_H

#include <QStandardItemModel>

struct ChannelUserData {
    QString name;
    int channel;
    int value;
    bool enabled;
};

Q_DECLARE_METATYPE(ChannelUserData);

class ChannelItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit ChannelItemModel(QObject *parent = nullptr);

    Qt::DropActions supportedDropActions() const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

};

#endif // CHANNELITEMMODEL_H
