function safeElementAttribute(node, attr) {
    try {
        return node.details[parent.index].element[attr]
    } catch (e) {
        return null;
    }
}
