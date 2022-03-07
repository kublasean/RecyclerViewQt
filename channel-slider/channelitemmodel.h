#ifndef CHANNELITEMMODEL_H
#define CHANNELITEMMODEL_H

#include <QAbstractListModel>

struct ChannelUserData {
    QString name;
    int channel;
    int value;
    bool enabled;
};

Q_DECLARE_METATYPE(ChannelUserData);

class ChannelItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChannelItemModel(QObject *parent = nullptr);

    Qt::DropActions supportedDropActions() const override;

    QStringList mimeTypes() const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int, Qt::Orientation, int) const override { return QVariant(); }

    int rowCount(const QModelIndex &) const override { return 512; }

    Qt::ItemFlags flags(const QModelIndex &index) const override;


private:
    ChannelUserData channels[512];

};

#endif // CHANNELITEMMODEL_H
