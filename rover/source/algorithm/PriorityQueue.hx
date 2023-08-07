package algorithm;

import haxe.ds.GenericStack;

class PriorityQueue extends GenericStack<Node> {
    public function new() {
        super();
    }

    public function addByF(item:Node):Void {
        if(head == null) {
            // If the stack is empty, just add the item.
            super.add(item);
            return;
        }

        var previous:GenericCell<Node> = null;
        var current:GenericCell<Node> = head;

        // Iterate until we find the correct spot based on the f value.
        while(current != null && item.f > current.elt.f) {
            previous = current;
            current = current.next;
        }

        // If the previous node is null, we're inserting at the start.
        if(previous == null) {
            head = new GenericCell<Node>(item, head);
        }else {
            var newNode = new GenericCell<Node>(item, current);
            previous.next = newNode;
        }
    }

    public function contains(n:Node):Bool {
        for(node in iterator()) {
            if(node == n)return true;
        }

        return false;
    }

    public function popN():Node {
        var lowestFNode = head.elt;
        var lowestF = lowestFNode.g + lowestFNode.h;
        var current:GenericCell<Node> = head;
        var previous:GenericCell<Node> = null;

        while(current != null) {
            var f = current.elt.g + current.elt.h;
            if(f < lowestF) {
                lowestF = f;
                lowestFNode = current.elt;
                previous = current;
            }
            current = current.next;
        }

        if(previous == null) {
            head = head.next;
        } else {
            previous.next = previous.next.next;
        }

        return lowestFNode;
    }

    public function clear():Void {
        for(node in iterator()) {
            remove(node);
        }
    }
}