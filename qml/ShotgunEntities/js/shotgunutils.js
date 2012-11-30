function safeElementAttribute(node, attr) {
    try {
        return node.elements[releaseSeq.ShotgunOperation.index][attr]
    } catch (e) {
        return null;
    }
}
