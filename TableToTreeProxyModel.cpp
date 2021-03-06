#include "TableToTreeProxyModel.h"

#include <QItemSelection>
#include <QSqlQueryModel>
#include <QDebug>
#include <QStringList>

class TreeNode {
public:
    TreeNode(int row=-1, int column=-1, TreeNode *parent=0)
        : row(row), column(column), parent(parent)
    {
        if (parent)
            parent->children.append(this);
    };
    ~TreeNode()
    {
        qDeleteAll(children);
    }
    int row;
    int column;
    TreeNode *parent;
    QList<TreeNode*> children;
    QString text;
};

TableToTreeProxyModel::TableToTreeProxyModel(QObject *parent)
    : QAbstractProxyModel(parent), rootNode(0)
{
}

TableToTreeProxyModel::~TableToTreeProxyModel()
{
    foreach(QList<TreeNode*> rowNodes, tableNodes)
        rowNodes.clear();
    tableNodes.clear();
    delete rootNode;
}

QModelIndex TableToTreeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    TreeNode *node = tableNodes.at(sourceIndex.column()).at(sourceIndex.row());
    Q_ASSERT(node);
    if (!node)
        return QModelIndex();

    Q_ASSERT(false);
    return index(sourceIndex.row(), 0);
}

QModelIndex TableToTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();

    const TreeNode *node = static_cast<TreeNode*>(proxyIndex.internalPointer());
    Q_ASSERT(node);
    if (!node)
        return QModelIndex();

    return sourceModel()->index(node->row, node->column);
}

QVariant TableToTreeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    const TreeNode *node = static_cast<TreeNode*>(proxyIndex.internalPointer());
    Q_ASSERT(node);
    if (!node)
        return QVariant();

    const QModelIndex index = sourceModel()->index(node->row, node->column);
    return sourceModel()->data(index, role);
}

QModelIndex TableToTreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    const TreeNode *parentNode = static_cast<TreeNode*>(parent.internalPointer());
    if (!parentNode)
        parentNode = rootNode;

    TreeNode *node = parentNode->children.at(row);
    Q_ASSERT(node);
    if (!node)
        return QModelIndex();

    return createIndex(row, column, node);
}

QModelIndex TableToTreeProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    const TreeNode *childNode = static_cast<TreeNode*>(child.internalPointer());
    Q_ASSERT(childNode);
    if (!childNode)
        return QModelIndex();

    TreeNode *parentNode = childNode->parent;
    Q_ASSERT(parentNode);
    if (!parentNode)
        return QModelIndex();
    if (parentNode == rootNode)
        return QModelIndex();

    return createIndex(parentNode->row, parentNode->column, parentNode);
}

int TableToTreeProxyModel::rowCount(const QModelIndex &parent) const
{
    const TreeNode *node = static_cast<TreeNode*>(parent.internalPointer());
    if (!node)
        node = rootNode;

    return node->children.size();
}

int TableToTreeProxyModel::columnCount(const QModelIndex &) const
{
    return 1;
}

bool TableToTreeProxyModel::hasChildren(const QModelIndex &parent) const
{
    const TreeNode *node = static_cast<TreeNode*>(parent.internalPointer());
    if (!node)
        node = rootNode;

    return node->children.size() > 0;
}

void TableToTreeProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (this->sourceModel()) {
        disconnect(this->sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                   this, SLOT(reset()));

        disconnect(this->sourceModel(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                   this, SLOT(reset()));

        //    disconnect(this->sourceModel(), SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
        //               this, SLOT(reset()));

        disconnect(this->sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this, SLOT(reset()));

        //    disconnect(this->sourceModel(), SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
        //               this, SLOT(reset()));

        disconnect(this->sourceModel(), SIGNAL(columnsInserted(QModelIndex,int,int)),
                   this, SLOT(reset()));

        //    disconnect(this->sourceModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
        //               this, SLOT(reset()));

        disconnect(this->sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                   this, SLOT(reset()));

        //    disconnect(this->sourceModel(), SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
        //               this, SLOT(reset()));

        disconnect(this->sourceModel(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
                   this, SLOT(reset()));

        //    disconnect(this->sourceModel(), SIGNAL(layoutAboutToBeChanged()),
        //               this, SLOT(reset()));

        disconnect(this->sourceModel(), SIGNAL(layoutChanged()),
                   this, SLOT(reset()));

        //    disconnect(this->sourceModel(), SIGNAL(modelAboutToBeReset()), this, SLOT(_q_sourceAboutToBeReset()));
        //    disconnect(this->sourceModel(), SIGNAL(modelReset()), this, SLOT(_q_sourceReset()));
    }

    QAbstractProxyModel::setSourceModel(sourceModel);
    reset();

    connect(this->sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(reset()));

    connect(this->sourceModel(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SLOT(reset()));

//    connect(this->sourceModel(), SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
//            this, SLOT(reset()));

    connect(this->sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(reset()));

//    connect(this->sourceModel(), SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
//            this, SLOT(reset()));

    connect(this->sourceModel(), SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SLOT(reset()));

//    connect(this->sourceModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
//            this, SLOT(reset()));

    connect(this->sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(reset()));

//    connect(this->sourceModel(), SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
//            this, SLOT(reset()));

    connect(this->sourceModel(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SLOT(reset()));

//    connect(this->sourceModel(), SIGNAL(layoutAboutToBeChanged()),
//            this, SLOT(reset()));

    connect(this->sourceModel(), SIGNAL(layoutChanged()),
            this, SLOT(reset()));

//    connect(this->sourceModel(), SIGNAL(modelAboutToBeReset()), this, SLOT(_q_sourceAboutToBeReset()));
//    connect(this->sourceModel(), SIGNAL(modelReset()), this, SLOT(_q_sourceReset()));
}

void TableToTreeProxyModel::reset()
{
    beginResetModel();

    foreach(QList<TreeNode*> rowNodes, tableNodes)
        rowNodes.clear();
    tableNodes.clear();
    delete rootNode;

    QAbstractProxyModel::reset();

    rootNode = new TreeNode;

    for (int row=0; row < sourceModel()->rowCount(); ++row) {
        QList<TreeNode*> rowNodes;
        TreeNode *previousNode = rootNode;
        for (int column=0; column < sourceModel()->columnCount(); ++column) {
            const QString &nodeText = sourceModel()->data(sourceModel()->index(row, column)).toString();
            TreeNode *node = 0;
            foreach (TreeNode *siblingNode, previousNode->children) {
                Q_ASSERT(siblingNode);
                if (!siblingNode)
                    continue;
                if (siblingNode->text == nodeText) {
                    node = siblingNode;
                    break;
                }
            }

            if (!node) {
                node = new TreeNode(row, column, previousNode);
                node->text = nodeText;
            }
            rowNodes.insert(column, node);
            previousNode = node;
        }
        tableNodes.insert(row, rowNodes);
    }

    endResetModel();
}
