function safeElementAttribute(node, attr) {
    try {
        return node.details[releaseSeq.ShotgunOperation.index].element[attr]
    } catch (e) {
        return null;
    }
}
