package algorithm;

/*
* It took me 12 hours to realize I need one of these.
*/

class PriorityQueue<T> {
    private var data:Array<T>;
    private var compare:T->T->Int;

    public function new(compare:T->T->Int) {
        this.data = [];
        this.compare = compare;
    }

    public function add(item:T):Void {
        var position = 0;

        for (i in 0...this.data.length) {
            if (this.compare(item, this.data[i]) > 0) {
                position = i + 1;
            } else {
                break;
            }
        }

        this.data.insert(position, item);
    }

    public function pop():Null<T> {
        return this.data.shift();
    }

    public function peek():Null<T> {
        return this.data[0];
    }

    public function isEmpty():Bool {
        return this.data.length == 0;
    }

    public function contains(item:T):Bool {
        return this.data.contains(item);
    }
}