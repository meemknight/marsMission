package shop;

class ShopQueue {
    private var heap:Array<Market>;
    private var compare:Market->Market->Int;

    public function new(compare:Market->Market->Int) {
        this.heap = [];
        this.compare = compare;
    }

    public function first():Market {
        return heap[0];
    }

    public function push(item:Market):Void {
        heap.push(item);
        var i = heap.length - 1;
        while (i > 0 && compare(heap[i], heap[parent(i)]) < 0) {
            swap(i, parent(i));
            i = parent(i);
        }
    }

    public function pop():Market {
        if(isEmpty())throw "Queue is empty!";
        
        var root = heap[0];
        heap[0] = heap.pop();
        
        var i = 0;
        while(true) {
            var l = left(i);
            var r = right(i);
            var smallest = i;
            
            if(l < heap.length && compare(heap[l], heap[smallest]) < 0) smallest = l;
            if(r < heap.length && compare(heap[r], heap[smallest]) < 0) smallest = r;
            
            if(smallest != i) {
                swap(i, smallest);
                i = smallest;
            }else {
                break;
            }
        }

        return root;
    }

    public function isEmpty():Bool {
        return heap.length == 0;
    }

    private function parent(i:Int):Int {
        return (i - 1) >> 1;
    }

    private function left(i:Int):Int {
        return (i << 1) + 1;
    }

    private function right(i:Int):Int {
        return (i << 1) + 2;
    }

    private function swap(i:Int, j:Int):Void {
        var temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
}