"""
This program reads a file with stack trace data (in the format that is written
by the stacktrace_example program in this repository). It parses the data and
creates a call graph, which is then written to a DOT (GraphViz) file.
"""
import os
import re
import sys
from collections import defaultdict
from dataclasses import dataclass, field, fields
from enum import Enum, auto
from typing import Dict, Optional, TextIO
from toolz import sliding_window
import pydot

VECTOR_RX = re.compile(r"std::__1::vector<([a-zA-Z:]+), std::__1::allocator<\1>>")


@dataclass(frozen=True)
class Frame:
    """A single frame observed in a call stack. It contains the full name of
    the function, and the library in which the function was found."""

    function: str
    library: str

    def dot_string(self):
        """Return a string to be used as the DOT file label for this frame."""
        result = f"{self.function}\\nlibrary: {self.library}"
        return result


@dataclass
class CallPath:
    """CallPath represents a unique full call path in a program.
    It contains a list of frames, and a count of the number of times this exact
    path was observed."""

    frames: list[Frame] = field(default_factory=list)
    ncalls: int = 0

    def append(self, frame: Frame):
        """Append the given frame to this CallPath."""
        self.frames.append(frame)


@dataclass(frozen=True)
class Edge:
    """Edge represents an edge in call graph. It contains the id of the
    caller and the id of the callee."""

    caller: int
    callee: int

    def __iter__(self):
        """Iterate over the fields of the Edge. This is here so that we can
        use unpacking, which requires the unpacked thing be iterable."""
        for some_field in fields(self):
            yield getattr(self, some_field.name)


class Graph:
    """Graph represents the full call graph collection for a program run."""

    def __init__(self):
        """Initialize an empty Graph."""
        self.num_funcs = 0
        self.funcs: Dict[Frame, int] = {}
        self.edges: defaultdict[Edge, int] = defaultdict(int)

    def record_function(self, frame: Frame) -> int:
        """Record this frame as the observation of some function in some
        library. Each function will be given a unique function id.

        Return the id of the function that was recorded."""
        if frame not in self.funcs:
            self.num_funcs += 1
            self.funcs[frame] = self.num_funcs
            return self.num_funcs
        return self.funcs[frame]

    def get_function_id(self, frame: Frame) -> int:
        """Return the function id for the given frame."""
        return self.funcs[frame]

    def append(self, path: CallPath):
        """Append a new CallPath to the graph."""
        ncalls = path.ncalls
        for callee, caller in sliding_window(2, path.frames):
            caller_id = self.record_function(caller)
            callee_id = self.record_function(callee)
            self.edges[Edge(caller_id, callee_id)] += ncalls

    def print(self, ofile):
        """Print the graph in DOT format to the provided (open) file."""
        dot = pydot.Dot("callgraph", graph_type="digraph")
        for func, func_id in self.funcs.items():
            node = pydot.Node(f"{func_id}", label=f"{func.dot_string()}", shape="box")
            dot.add_node(node)

        for (caller, callee), ncalls in self.edges.items():
            edge = pydot.Edge(caller, callee, label=f"{ncalls}")
            dot.add_edge(edge)

        ofile.write(dot.to_string())


class State(Enum):
    """Enumeration of legal parser states."""

    BUILDING = auto()
    DONE = auto()
    ERROR = auto()
    READY = auto()


def generate_call_stacks(ifile: TextIO):
    """A generator that reads from ifile and yields CallPath objects."""
    current_path: Optional[CallPath] = None
    current_state = State.READY

    for line in ifile:
        line = line.strip()
        if len(line) == 0:
            # We skip blank lines. Usualy good practice, but we should actually
            # never see one.
            continue
        if line.startswith("###ncalls"):
            # These assertions will only fire on malformed files. We could work
            # harder and create a ParseError exception class to raise.
            assert current_state is State.BUILDING
            assert current_path is not None
            _, ncalls = line.split(" ")
            current_path.ncalls = int(ncalls)
            yield current_path
            current_state = State.READY
            current_path = None
        else:
            # The only other sort of line in our file is a record of a
            # stack frame.
            if current_state is State.READY:
                current_path = CallPath()
                current_state = State.BUILDING
            assert current_state is State.BUILDING
            assert current_path is not None
            function_name, library_name = parse_frame_line(line)
            current_path.append(Frame(function_name, library_name))


def parse_frame_line(line):
    """Parse the text from a 'frame line' into a pleasant-to-read function
    name and library name form.

    Note that the "cleanup" done, using regular expressions, is not powerful
    enough to handle nested templates. If we use this in a context where that is
    needed, then we can no longer use regular expressions to clean up the
    templates."""
    function_name, library_name = line.split(" in ")

    function_name = function_name.replace("ROOT::Minuit2", "RM")

    # Fix up vectors of simple types.
    function_name = VECTOR_RX.sub(r"vector<\1>", function_name)

    library_name = os.path.basename(library_name)
    return function_name, library_name


def process(file: TextIO):
    """Process the trace data in the open file `file`, and create a DOT
    (graphviz) file showing the call graph."""
    print(f"Processing file {file.name}")
    graph = Graph()
    for call_stack in generate_call_stacks(file):
        graph.append(call_stack)

    output_name = f"{file.name}.dot"
    with open(output_name, mode="w", encoding="utf-8") as output_file:
        graph.print(output_file)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Please specify the file to read")
        sys.exit(1)
    with open(sys.argv[1], encoding="utf-8") as trace_file:
        process(trace_file)
