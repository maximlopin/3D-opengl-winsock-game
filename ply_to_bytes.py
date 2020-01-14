import sys
import os
from array import array

def get_vertices_and_faces(lines):

    for i, l in enumerate(lines):
        if l.startswith('element vertex'):
            n_vertices = int(l.split(' ')[-1])
            element_vertex_idx = i
            break
    else:
        raise Exception('Failed to find element vertex')

    for i, l in enumerate(lines):
        if l.startswith('element face'):
            n_faces = int(l.split(' ')[-1])
            element_face_idx = i
            break
    else:
        raise Exception('Failed to find element face')

    floats_per_vertex = 0

    for l in lines[element_vertex_idx + 1:]:
        if not l.startswith('property'):
            break
        
        assert l.startswith('property float')

        floats_per_vertex += 1
    
    
    *_, tmp = lines[element_face_idx + 1].split(' ')
    assert tmp == 'vertex_indices', 'unexpected element face property'

    tmp = lines[element_face_idx + 2]
    assert tmp == 'end_header', 'expected end_header, found {}'.format(tmp)

    vertices = []

    for l in lines[element_face_idx + 3: element_face_idx + 3 + n_vertices]:
        vertices.append(tuple(float(x) for x in l.split(' ')))

    assert len(vertices) == n_vertices

    faces = []

    for l in lines[element_face_idx + 3 + n_vertices:]:
        if len(l) == 0:
            break

        n, *indices = l.split(' ')

        n = int(n)

        assert n == len(indices)

        if n == 4:
            a, b, c, d = indices
            faces.append((int(a), int(b), int(c)))
            faces.append((int(c), int(d), int(a)))
        elif n == 3:
            a, b, c = indices
            faces.append((int(a), int(b), int(c)))
        else:
            raise ValueError('unexpected number of indices per face (must be 3 or 4)')
        
    return floats_per_vertex, vertices, faces

if __name__ == '__main__':
    assert len(sys.argv) >= 2, 'Usage: ./python {} asd.ply'.format(__file__)

    filename = sys.argv[-1]

    assert os.path.isfile(filename), '{} is not a file'.format(filename)

    with open(filename, 'r') as f:

        try:
            lines = f.read().split('\n')
        except UnicodeDecodeError:
            exit('Failed to decode {}'.format(filename))

        assert lines and lines[0] == 'ply', '{} is not a ply file'.format(filename)

        floats_per_vertex, vertices, faces = get_vertices_and_faces(lines)

    float_ = 'f'
    uint32_t = 'L'

    sizeof_float = 4
    sizeof_uint32_t = 4

    with open('{}.bin'.format(filename), 'wb') as f:

        vertices_data_size = len(vertices) * floats_per_vertex * sizeof_float
        indices_data_size = len(faces) * 3 * sizeof_uint32_t

        array(uint32_t, [vertices_data_size]).tofile(f)
        array(uint32_t, [indices_data_size]).tofile(f)

        # Vertices
        floats = [float(float_) for vert in vertices for float_ in vert]
        array(float_, floats).tofile(f)
    
        # Faces
        ints = [int(int_) for face in faces for int_ in face]
        array(uint32_t, ints).tofile(f)
