// Copyright 2014 Ben Smith. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

var assert = require('assert'),
    module = require('../src/js/module.js'),
    type = require('../src/js/type.js');

describe('Module', function() {
  it('should look like dis', function() {
    var voidp = type.Pointer(type.void),
        intp = type.Pointer(type.int),
        mallocType = type.Function(voidp, [type.uint]),
        getIntType = type.Function(type.int, [intp]),
        m = module.Module(),
        p,
        x;

    m.$defineFunction('malloc', [module.Function(1, mallocType)]);
    m.$defineFunction('get', [module.Function(2, getIntType)]);

    assert.deepEqual(m.$getMessage(), {handles:{}, commands:[]});

    p = m.malloc(4).cast(intp);

    assert.deepEqual(m.$getMessage(), {
      handles: { 1: 4 },
      commands: [ {id: 1, args: [1], ret: 2} ]
    });

    x = m.get(p);

    assert.deepEqual(m.$getMessage(), {
      handles: { 1: 4 },
      commands: [
        {id: 1, args: [1], ret: 2},
        {id: 2, args: [2], ret: 3}
      ]
    });
  });

  it('should allow creation of handles', function() {
    var m = module.Module(),
        h1,
        h2;

    h1 = m.$handle(4);
    h2 = m.$handle(4, type.float);

    assert.strictEqual(h1.type, type.schar);
    assert.strictEqual(h2.type, type.float);

    assert.deepEqual(m.$getMessage(), {
      handles: {
        1: 4,
        2: 4
      },
      commands: []
    });
  });

  it('should allow use of explicitly-created handles', function() {
    var addType = type.Function(type.int, [type.int, type.int]),
        m = module.Module(),
        h;

    m.$defineFunction('add', [module.Function(1, addType)]);
    h = m.$handle(4);
    m.add(h, h);

    assert.deepEqual(m.$getMessage(), {
      handles: { 1: 4 },
      commands: [
        {id: 1, args: [1, 1], ret: 2}
      ]
    });
  });
});
