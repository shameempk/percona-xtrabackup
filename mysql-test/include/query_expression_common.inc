CREATE TABLE t(a INT);
CREATE TABLE t1(a INT);
CREATE TABLE r(a INT);
INSERT INTO t VALUES (1),(2),(3);
INSERT INTO t1 VALUES (1),(2);
INSERT INTO r VALUES (2);
ANALYZE TABLE t, t1, r;

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM t UNION SELECT * FROM t1)  UNION ALL (SELECT * FROM r);

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM t UNION SELECT * FROM t1)  UNION ALL SELECT * FROM r
   UNION ALL SELECT * FROM t;

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM t UNION SELECT * FROM t1)  UNION ALL SELECT * FROM r
   UNION DISTINCT SELECT * FROM t;

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM t UNION SELECT * FROM t1)  UNION ALL SELECT * FROM r
   UNION ALL (SELECT * FROM t UNION DISTINCT SELECT * FROM r);

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM t UNION SELECT * FROM t1)  UNION ALL SELECT * FROM r
   UNION ALL (SELECT * FROM t UNION ALL SELECT * FROM r);

DROP TABLE t, t1, r;

--echo #
--echo # INTERSECT and EXCEPT support
--echo #

CREATE TABLE r(a INT);
CREATE TABLE s(a INT);
CREATE TABLE t(a INT);
INSERT INTO r VALUES (1),(2),(3);
INSERT INTO s VALUES (1),(2);
INSERT INTO t VALUES (2);
ANALYZE TABLE r, s, t;

--echo #
--echo # Test operator precedence
--echo #

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r UNION ALL SELECT * FROM s) INTERSECT ALL SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r UNION ALL SELECT * FROM s INTERSECT ALL SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r EXCEPT ALL SELECT * FROM s) INTERSECT ALL SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r EXCEPT ALL SELECT * FROM s INTERSECT ALL SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r EXCEPT ALL SELECT * FROM s) UNION ALL SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r EXCEPT ALL SELECT * FROM s UNION ALL SELECT * FROM t;

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r UNION DISTINCT SELECT * FROM s) INTERSECT DISTINCT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r UNION DISTINCT SELECT * FROM s INTERSECT DISTINCT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s) INTERSECT DISTINCT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s INTERSECT DISTINCT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s) UNION DISTINCT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s UNION DISTINCT SELECT * FROM t;

--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r UNION SELECT * FROM s) INTERSECT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r UNION SELECT * FROM s INTERSECT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r EXCEPT SELECT * FROM s) INTERSECT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r EXCEPT SELECT * FROM s INTERSECT SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   (SELECT * FROM r EXCEPT SELECT * FROM s) UNION SELECT * FROM t;
--replace_regex $elide_costs
EXPLAIN FORMAT = tree
   SELECT * FROM r EXCEPT SELECT * FROM s UNION SELECT * FROM t;

--echo #
--echo # Bug#33905918 Server crashes after using explain of a query with except/intersect.
--echo #
EXPLAIN SELECT * FROM r UNION ALL SELECT * FROM s;
EXPLAIN SELECT * FROM r EXCEPT ALL SELECT * FROM s;
EXPLAIN SELECT * FROM r INTERSECT ALL SELECT * FROM s;
EXPLAIN SELECT * FROM r UNION DISTINCT SELECT * FROM s;
EXPLAIN SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s;
EXPLAIN SELECT * FROM r INTERSECT DISTINCT SELECT * FROM s;
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json SELECT * FROM r UNION ALL SELECT * FROM s;
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json SELECT * FROM r EXCEPT ALL SELECT * FROM s;
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json SELECT * FROM r INTERSECT ALL SELECT * FROM s;
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json SELECT * FROM r UNION DISTINCT SELECT * FROM s;
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s;
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json SELECT * FROM r INTERSECT DISTINCT SELECT * FROM s;
# Bug#34455923
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT=json SELECT * FROM (SELECT 1 INTERSECT SELECT 2) AS dt;
--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT=json SELECT * FROM (SELECT 1 EXCEPT SELECT 2) AS dt;

--replace_regex $elide_json_costs
EXPLAIN (SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s) UNION DISTINCT SELECT * FROM t;
--replace_regex $elide_json_costs
EXPLAIN (SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s) UNION DISTINCT
        (SELECT * FROM t INTERSECT DISTINCT SELECT * FROM t);

--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json
   (SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s) UNION DISTINCT SELECT * FROM t;

--skip_if_hypergraph  # Different JSON for hypergraph
--replace_regex $elide_json_costs
EXPLAIN FORMAT = json
   (SELECT * FROM r EXCEPT DISTINCT SELECT * FROM s) UNION DISTINCT
   (SELECT * FROM t INTERSECT DISTINCT SELECT * FROM t);

DROP TABLE r, s, t;

--echo #
--echo # Check that more set ops don't interfere with
--echo # WITH RECURSIVE
--echo #

--error ER_CTE_RECURSIVE_REQUIRES_UNION
WITH RECURSIVE qn AS
(SELECT 1 AS n, 1 AS un, 1 AS unp1 EXCEPT ALL
   SELECT 1+n, unp1, un+unp1 FROM qn WHERE n<10)
SELECT * FROM qn;

--error ER_CTE_RECURSIVE_REQUIRES_UNION
WITH RECURSIVE qn AS
(SELECT 1 AS n, 1 AS un, 1 AS unp1 INTERSECT ALL
   SELECT 1+n, unp1, un+unp1 FROM qn WHERE n<10)
SELECT * FROM qn;

CREATE TABLE t(n int, un INT, unp1 INT);
INSERT INTO t VALUES (1, 1, 1);
ANALYZE TABLE t;

--echo Non-union set op in seed works
WITH RECURSIVE qn AS
( SELECT * FROM t INTERSECT
  VALUES ROW(1, 1, 1) UNION ALL
 SELECT 1+n, unp1, un+unp1 FROM qn WHERE n<10)
SELECT * FROM qn;

--error ER_CTE_RECURSIVE_NOT_UNION
WITH RECURSIVE qn AS
( SELECT * FROM t UNION ALL
  (SELECT 1,1,1 INTERSECT SELECT 1+n, unp1, un+unp1 FROM qn WHERE n < 20)) SELECT * FROM qn;

--error ER_CTE_RECURSIVE_NOT_UNION
WITH RECURSIVE qn AS
( SELECT * FROM t UNION ALL
  (SELECT 1+n, unp1, un+unp1 FROM qn WHERE n<10 INTERSECT SELECT 1,1,1)) SELECT * FROM qn;

# had a bug for a while: EXCEPT was accidentally replaced with UNION
# in Query_result_union::create_result_table
WITH RECURSIVE cte AS
( ( (SELECT 1 AS n UNION SELECT 2) EXCEPT SELECT 2)  UNION ALL
  SELECT n+1  FROM cte WHERE n<100)
SELECT n FROM cte LIMIT 10;

DROP TABLE t;

CREATE TABLE t1(i INT);
CREATE TABLE t2(i INT);
CREATE TABLE t3(i INT);
INSERT INTO t1 VALUES (1),(1),(1);
INSERT INTO t2 VALUES (2),(2),(1),(1);
INSERT INTO t3 VALUES (2),(3),(3),(1),(1);
ANALYZE TABLE t1, t2, t3;

--echo # DISTINCT. Note: a mix of ALL and DISTINCT also gives all DISTINCT
SELECT * FROM t1 INTERSECT DISTINCT SELECT * FROM t2;
SELECT * FROM t2 INTERSECT DISTINCT SELECT * FROM t1;

SELECT * FROM t1 INTERSECT ALL SELECT * FROM t2 INTERSECT ALL SELECT * FROM t3;
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1 INTERSECT ALL SELECT * FROM t3;
SELECT * FROM t1 INTERSECT ALL SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2;
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t1 INTERSECT ALL SELECT * FROM t2;
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t3 INTERSECT ALL SELECT * FROM t1;
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1;

SELECT * FROM t1 INTERSECT DISTINCT SELECT * FROM t2 INTERSECT ALL SELECT * FROM t3;
SELECT * FROM t2 INTERSECT DISTINCT SELECT * FROM t1 INTERSECT ALL SELECT * FROM t3;
SELECT * FROM t1 INTERSECT DISTINCT SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2;
SELECT * FROM t3 INTERSECT DISTINCT SELECT * FROM t1 INTERSECT ALL SELECT * FROM t2;
SELECT * FROM t2 INTERSECT DISTINCT SELECT * FROM t3 INTERSECT ALL SELECT * FROM t1;
SELECT * FROM t3 INTERSECT DISTINCT SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1;

SELECT * FROM t1 INTERSECT ALL SELECT * FROM t2 INTERSECT DISTINCT SELECT * FROM t3;
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1 INTERSECT DISTINCT SELECT * FROM t3;
SELECT * FROM t1 INTERSECT ALL SELECT * FROM t3 INTERSECT DISTINCT SELECT * FROM t2;
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t1 INTERSECT DISTINCT SELECT * FROM t2;
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t3 INTERSECT DISTINCT SELECT * FROM t1;
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2 INTERSECT DISTINCT SELECT * FROM t1;

SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t3;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t3;
SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t2;
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t2;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t1;
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t1;

SELECT * FROM t1 EXCEPT    DISTINCT SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t3;
SELECT * FROM t2 EXCEPT    DISTINCT SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t3;
SELECT * FROM t1 EXCEPT    DISTINCT SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t2;
SELECT * FROM t3 EXCEPT    DISTINCT SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t2;
SELECT * FROM t2 EXCEPT    DISTINCT SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t1;
SELECT * FROM t3 EXCEPT    DISTINCT SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t1;

SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t2 EXCEPT    DISTINCT SELECT * FROM t3;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t1 EXCEPT    DISTINCT SELECT * FROM t3;
SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t3 EXCEPT    DISTINCT SELECT * FROM t2;
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t1 EXCEPT    DISTINCT SELECT * FROM t2;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t3 EXCEPT    DISTINCT SELECT * FROM t1;
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t2 EXCEPT    DISTINCT SELECT * FROM t1;

--echo # ALL
SELECT * FROM t1 INTERSECT ALL SELECT * FROM t2;
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1;
SELECT * FROM t1 INTERSECT ALL SELECT * FROM t3;
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t1;
--sorted_result
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t3;
--sorted_result
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2;

SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t2;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t1;
SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t3;
--sorted_result
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t1;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t3;
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t2;

SELECT * FROM t1 INTERSECT ALL SELECT * FROM t2 INTERSECT ALL SELECT * FROM t3;
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1 INTERSECT ALL SELECT * FROM t3;
SELECT * FROM t1 INTERSECT ALL SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2;
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t1 INTERSECT ALL SELECT * FROM t2;
SELECT * FROM t2 INTERSECT ALL SELECT * FROM t3 INTERSECT ALL SELECT * FROM t1;
SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1;

SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t3;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t3;
SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t2;
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t1 EXCEPT    ALL SELECT * FROM t2;
SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t1;
SELECT * FROM t3 EXCEPT    ALL SELECT * FROM t2 EXCEPT    ALL SELECT * FROM t1;

TRUNCATE t1;
TRUNCATE t2;
TRUNCATE t3;
INSERT INTO t3 VALUES (3),(3),(2),(2),(1),(1);
INSERT INTO t2 VALUES (2),(1),(1);
INSERT INTO t1 VALUES (1);
ANALYZE TABLE t1, t2, t3;

--echo # EXCEPT is not right associative, so make a right parentheses
--echo # nest to evaluate it first if wanted. Also test mix of
--echo # DISTINCT & ALL.

--echo #
--echo # DISTINCT
--echo #
let $query = SELECT * FROM t3 EXCEPT SELECT * FROM t2 EXCEPT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 EXCEPT SELECT * FROM t2) EXCEPT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 EXCEPT (SELECT * FROM t2 EXCEPT SELECT * FROM t1);
--sorted_result
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

--echo #
--echo # ALL
--echo #
let $query = SELECT * FROM t3 EXCEPT ALL SELECT * FROM t2 EXCEPT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 EXCEPT ALL SELECT * FROM t2) EXCEPT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 EXCEPT ALL (SELECT * FROM t2 EXCEPT ALL SELECT * FROM t1);
--sorted_result
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

--echo #
--echo # Mix of DISTINCT and ALL
--echo #
let $query = SELECT * FROM t3 EXCEPT ALL SELECT * FROM t2 EXCEPT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 EXCEPT ALL SELECT * FROM t2) EXCEPT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 EXCEPT ALL (SELECT * FROM t2 EXCEPT SELECT * FROM t1);
--sorted_result
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query = SELECT * FROM t3 EXCEPT SELECT * FROM t2 EXCEPT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 EXCEPT SELECT * FROM t2) EXCEPT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 EXCEPT (SELECT * FROM t2 EXCEPT ALL SELECT * FROM t1);
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

TRUNCATE t1;
TRUNCATE t2;
TRUNCATE t3;
INSERT INTO t3 VALUES (3),(3),(2),(2),(1),(1);
INSERT INTO t2 VALUES (2),(2),(1),(1);
INSERT INTO t1 VALUES (1),(1);
ANALYZE TABLE t1, t2, t3;

--echo # INTERSECT is left and right associative. If evaluation is
--echo # INTERSECT ALL, each operation can only handle two operands,
--echo # so for N operands we must set up N-1 operation nodes. If
--echo # evaluation is INTERSECT UNIQUE, collapse all operands and use
--echo # a single operation node. The restriction for INTERSECT ALL is
--echo # due to the fact we need an extra read pass to check counters
--echo # after each right-hand operand has been processed.  Also, test
--echo # mix of DISTINCT & ALL: DISTINCT wins always.

--echo #
--echo # DISTINCT
--echo #
let $query = SELECT * FROM t3 INTERSECT SELECT * FROM t2 INTERSECT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 INTERSECT SELECT * FROM t2) INTERSECT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 INTERSECT (SELECT * FROM t2 INTERSECT SELECT * FROM t1);
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

--echo #
--echo # ALL
--echo #
let $query = SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2) INTERSECT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 INTERSECT ALL (SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1);
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

--echo #
--echo # Mix of DISTINCT and ALL
--echo #
let $query = SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2 INTERSECT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 INTERSECT ALL SELECT * FROM t2) INTERSECT SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 INTERSECT ALL (SELECT * FROM t2 INTERSECT SELECT * FROM t1);
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query = SELECT * FROM t3 INTERSECT SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = (SELECT * FROM t3 INTERSECT SELECT * FROM t2) INTERSECT ALL SELECT * FROM t1;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;
let $query = SELECT * FROM t3 INTERSECT (SELECT * FROM t2 INTERSECT ALL SELECT * FROM t1);
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

# can_materialize_directly_into_result bug
let $query =
SELECT * FROM (SELECT * FROM t3 UNION DISTINCT SELECT * FROM t3 UNION ALL SELECT * FROM t3) x1
INTERSECT
SELECT * FROM (SELECT * FROM t3 UNION DISTINCT SELECT * FROM t3 UNION ALL SELECT * FROM t3) x2
WHERE i > 1 ORDER BY i;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT * FROM (SELECT i FROM t2 INTERSECT SELECT i FROM t2 ORDER BY i) x1 WHERE i < 5;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

--echo #
--echo # Test LIMIT, OFFSET
--echo #
let $query =
SELECT i FROM t2 INTERSECT SELECT i FROM t2 LIMIT 1;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT i FROM t2 INTERSECT SELECT i FROM t2 LIMIT 1 OFFSET 1;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

--echo #
--echo # LIMIT, OFFSET with derived table
--echo #
let $query =
SELECT * FROM (SELECT i FROM t2 INTERSECT SELECT i FROM t2 LIMIT 1) t;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT * FROM (SELECT i FROM t1 INTERSECT SELECT i FROM t3 LIMIT 1) t;
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT * FROM (SELECT i FROM t2 INTERSECT SELECT i FROM t2) t LIMIT 1;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT * FROM (SELECT i FROM t2 INTERSECT SELECT i FROM t2 LIMIT 1 OFFSET 1) t;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT * FROM (SELECT i FROM t2 INTERSECT SELECT i FROM t2) t LIMIT 1 OFFSET 1 ;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

#
# Test of SQL_CALC_FOUND_ROW handling
#
let $query =
SELECT SQL_CALC_FOUND_ROWS i FROM t2 INTERSECT SELECT i FROM t2 LIMIT 1;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
SELECT FOUND_ROWS();

let $query =
SELECT SQL_CALC_FOUND_ROWS * FROM (SELECT i FROM t2 INTERSECT SELECT i FROM t2) t LIMIT 1;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
SELECT FOUND_ROWS();

let $query =
SELECT SQL_CALC_FOUND_ROWS * FROM (SELECT i FROM t2 INTERSECT SELECT i FROM t2 LIMIT 1) t;
# Non-deterministic because ORDER BY is missing. Both 1 and 2 is correct.
--replace_regex /^[12]$/CORRECT/
eval $query;
SELECT FOUND_ROWS();

#
# Test empty left operand short circuit
#
let $query1 =
(SELECT i FROM t1 WHERE i > 100) INTERSECT SELECT i FROM t2;

let $query2 =
(SELECT i FROM t1 LIMIT 0) INTERSECT SELECT i FROM t2;

eval $query1;
--replace_regex $elide_costs_and_time_and_row_estimate
eval EXPLAIN analyze $query1;

eval $query2;
--replace_regex $elide_costs_and_time_and_row_estimate
eval EXPLAIN analyze $query2;

DROP TABLE t1, t2, t3;


--echo #
--echo # Test of T101 "enhanced nullability determination". To interpret results
--echo # read SQL 2014, Vol 2. section 7.17 <query expression>, SR 18 and 20.
--echo #
CREATE TABLE t1 AS SELECT 1 AS c1, 1 AS c2 UNION SELECT 2,2;
CREATE TABLE t2 AS SELECT 1 AS c1, 1 AS c2 UNION SELECT null, null;
CREATE TABLE t3 AS SELECT null AS c1, null AS c2 UNION SELECT 2,2;
CREATE TABLE t4 AS SELECT null AS c1, null AS c2 UNION SELECT null, null;
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
SHOW CREATE TABLE t4;
DROP TABLE t1, t2, t3, t4;
CREATE TABLE t1 AS SELECT 1 AS c1, 1 AS c2 INTERSECT SELECT 2,2;
CREATE TABLE t2 AS SELECT 1 AS c1, 1 AS c2 INTERSECT SELECT null, null;
CREATE TABLE t3 AS SELECT null AS c1, null AS c2 INTERSECT SELECT 2,2;
CREATE TABLE t4 AS SELECT null AS c1, null AS c2 INTERSECT SELECT null, null;
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
SHOW CREATE TABLE t4;
DROP TABLE t1, t2, t3, t4;
CREATE TABLE t1 AS SELECT 1 AS c1, 1 AS c2 EXCEPT SELECT 2,2;
CREATE TABLE t2 AS SELECT 1 AS c1, 1 AS c2 EXCEPT SELECT null, null;
CREATE TABLE t3 AS SELECT null AS c1, null AS c2 EXCEPT SELECT 2,2;
CREATE TABLE t4 AS SELECT null AS c1, null AS c2 EXCEPT SELECT null, null;
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
SHOW CREATE TABLE t4;
DROP TABLE t1, t2, t3, t4;

--echo # Test T101 hierarchy correctness
--echo # EXCEPT on top, UNION in its right side
CREATE TABLE t1 AS SELECT 1 AS c1, 1 AS c2 EXCEPT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT null, null);
CREATE TABLE t2 AS SELECT 1 AS c1, 1 AS c2 EXCEPT
                  (SELECT null AS c1, null AS c2 UNION SELECT null, null);
CREATE TABLE t3 AS SELECT 1 AS c1, 1 AS c2 EXCEPT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

CREATE TABLE t1 AS SELECT null AS c1, null AS c2 EXCEPT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT null, null);
CREATE TABLE t2 AS SELECT null AS c1, null AS c2 EXCEPT
                  (SELECT null AS c1, null AS c2 UNION SELECT null, null);
CREATE TABLE t3 AS SELECT null AS c1, null AS c2 EXCEPT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

--echo # INTERSECT on top, UNION in its right side
CREATE TABLE t1 AS SELECT 1 AS c1, 1 AS c2 INTERSECT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT null, null);
CREATE TABLE t2 AS SELECT 1 AS c1, 1 AS c2 INTERSECT
                  (SELECT null AS c1, null AS c2 UNION SELECT null, null);
CREATE TABLE t3 AS SELECT 1 AS c1, 1 AS c2 INTERSECT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

CREATE TABLE t1 AS SELECT null AS c1, null AS c2 INTERSECT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT null, null);
CREATE TABLE t2 AS SELECT null AS c1, null AS c2 INTERSECT
                  (SELECT null AS c1, null AS c2 UNION SELECT null, null);
CREATE TABLE t3 AS SELECT null AS c1, null AS c2 INTERSECT
                  (SELECT 1 AS c1, 1 AS c2 UNION SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

--echo # UNION on top, INTERSECT in its right side
CREATE TABLE t1 AS SELECT 1 AS c1, 1 AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 INTERSECT SELECT null, null);
CREATE TABLE t2 AS SELECT 1 AS c1, 1 AS c2 UNION
                  (SELECT null AS c1, null AS c2 INTERSECT SELECT null, null);
CREATE TABLE t3 AS SELECT 1 AS c1, 1 AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 INTERSECT SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

CREATE TABLE t1 AS SELECT null AS c1, null AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 INTERSECT SELECT null, null);
CREATE TABLE t2 AS SELECT null AS c1, null AS c2 UNION
                  (SELECT null AS c1, null AS c2 INTERSECT SELECT null, null);
CREATE TABLE t3 AS SELECT null AS c1, null AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 INTERSECT SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

--echo # UNION on top, EXCEPT in its right side
CREATE TABLE t1 AS SELECT 1 AS c1, 1 AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 EXCEPT SELECT null, null);
CREATE TABLE t2 AS SELECT 1 AS c1, 1 AS c2 UNION
                  (SELECT null AS c1, null AS c2 EXCEPT SELECT null, null);
CREATE TABLE t3 AS SELECT 1 AS c1, 1 AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 EXCEPT SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

CREATE TABLE t1 AS SELECT null AS c1, null AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 EXCEPT SELECT null, null);
CREATE TABLE t2 AS SELECT null AS c1, null AS c2 UNION
                  (SELECT null AS c1, null AS c2 EXCEPT SELECT null, null);
CREATE TABLE t3 AS SELECT null AS c1, null AS c2 UNION
                  (SELECT 1 AS c1, 1 AS c2 EXCEPT SELECT 2,2);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;
SHOW CREATE TABLE t3;
DROP TABLE t1, t2, t3;

--echo # T101 for unary
CREATE TABLE t1 AS ((VALUES ROW (1, 1), ROW (2,2) ORDER BY column_0 LIMIT 2)
                    ORDER BY column_1 LIMIT 1);
CREATE TABLE t2 AS ((VALUES ROW (null, null), ROW (2,2)
                    ORDER BY column_0 LIMIT 2) ORDER BY column_1 LIMIT 1);
SHOW CREATE TABLE t1;
SHOW CREATE TABLE t2;

DROP TABLE t1, t2;

--echo #
--echo # Row count estimates. For UNION, the optimizer adds the
--echo # numbers of rows for the operands, (10, the worst case in
--echo # example below). For INTERSECT, the estimate should be the
--echo # lowest estimate of the operands (3). For EXCEPT it should be
--echo # estimate of the left operand (7, worst case no rows are
--echo # removed from the set).
--echo #
CREATE TABLE t1(a INT);
INSERT INTO t1 VALUES (1), (1), (2), (3), (2), (3), (3); # 7 rows
ANALYZE TABLE t1;

--replace_regex $elide_costs
--skip_if_hypergraph  # Different plan
EXPLAIN FORMAT=tree
SELECT * FROM t1 UNION     (SELECT * FROM t1 ORDER BY 1 LIMIT 3);

--replace_regex $elide_costs
--skip_if_hypergraph  # Different plan
EXPLAIN FORMAT=tree
SELECT * FROM t1 INTERSECT (SELECT * FROM t1 ORDER BY 1 LIMIT 3);

--replace_regex $elide_costs
--skip_if_hypergraph  # Different plan
EXPLAIN FORMAT=tree
SELECT * FROM t1 EXCEPT    (SELECT * FROM t1 ORDER BY 1 LIMIT 3);

DROP TABLE t1;

--echo #
--echo # Bug#34843764 Wrong Results when INTERSECT/EXCEPT combine with JOIN
--echo #
CREATE TABLE t1 (a INT, b INT);

INSERT INTO t1 VALUES (NULL, NULL), (1, NULL), (2, 3), (4, 5), (6, 5);
INSERT INTO t1 VALUES (NULL, NULL), (1, NULL), (2, 3), (4, 5), (6, 5);
INSERT INTO t1 VALUES (NULL, NULL), (1, NULL), (2, 3), (4, 5), (6, 5);

CREATE TABLE t2 (a INT, b INT);

INSERT INTO t2 VALUES (NULL, NULL), (1, NULL), (2, 3), (4, 5), (6, 5), (7, 5), (8, 9);
INSERT INTO t2 VALUES (NULL, NULL), (1, NULL), (2, 3), (4, 5), (6, 5), (7, 5), (8, 9);

CREATE TABLE t3 (a INT, b INT);

INSERT INTO t3 VALUES (NULL, NULL), (1, NULL), (2, 3), (4, 5);
INSERT INTO t3 VALUES (NULL, NULL), (1, NULL), (2, 3), (4, 5);
ANALYZE TABLE t1, t2, t3;

let $query =
SELECT *
FROM (SELECT a, b FROM t1 INTERSECT SELECT a, b FROM t3) AS x
     JOIN
     t2
     ON x.a = t2.a AND x.b = t2.b
ORDER BY 1, 2;

eval $query;
--skip_if_hypergraph  # Different plans.
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT *
FROM (SELECT a, b FROM t1 INTERSECT ALL SELECT a, b FROM t3) AS x
     JOIN
     t2
     ON x.a = t2.a AND x.b = t2.b
ORDER BY 1, 2;

eval $query;
--skip_if_hypergraph  # Different plans.
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT *
FROM (SELECT a, b FROM t1 EXCEPT SELECT a, b FROM t3) AS x
     JOIN
     t2
     ON x.a = t2.a AND x.b = t2.b
ORDER BY 1, 2;

eval $query;
--skip_if_hypergraph  # Different plans.
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

let $query =
SELECT *
FROM (SELECT a, b FROM t1 EXCEPT ALL SELECT a, b FROM t3) AS x
     JOIN
     t2
     ON x.a = t2.a AND x.b = t2.b
ORDER BY 1, 2;

eval $query;
--skip_if_hypergraph  # Different plans.
--replace_regex $elide_costs
eval EXPLAIN FORMAT=tree $query;

DROP TABLE t1, t2, t3;

--echo #
--echo # Bug#34704011 When using a single select in parenthesis, a
--echo #              global order by cannot be used
--echo #
CREATE TABLE t1 (id INT PRIMARY KEY AUTO_INCREMENT, d DATE);
CREATE TABLE t2 (id INT PRIMARY KEY AUTO_INCREMENT, d DATE);

INSERT INTO t1 (d) VALUES ('2020-01-01'), ('2021-04-21'), ('2022-03-02');
INSERT INTO t2 (d) VALUES ('2020-05-01'), ('2021-05-21'), ('2022-05-02');
ANALYZE TABLE t1, t2;

# The reference to t1 in the ORDER BY was flagged as an error
# which is strictly is according the SQL standard, but MySQL used to
# allow this before WL#11350, so make it work again.
( SELECT t1.d, t2.d
  FROM t1 INNER JOIN t2 USING (id) )
ORDER BY t1.d;

(( SELECT t1.d, t2.d
  FROM t1 INNER JOIN t2 USING (id) ))
ORDER BY t1.d;

--error ER_BAD_FIELD_ERROR
(( SELECT t1.d, t2.d
  FROM t1 INNER JOIN t2 USING (id) ) LIMIT 1)
ORDER BY t1.d;

DROP TABLE t1, t2;
