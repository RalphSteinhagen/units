<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# compressibility Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_compressibility["<b><a href="../../systems/isq/#compressibility" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::compressibility</a></b><br><i>(inverse(<a href="../../systems/isq/#volume" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">volume</a>) * (<a href="../../systems/isq/#volume" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">volume</a> / <a href="../../systems/isq/#pressure" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">pressure</a>))</i>"]
    isq_compressibility -.- isq_isentropic_compressibility
    isq_compressibility -.- isq_isothermal_compressibility
    subgraph kind_isq_isentropic_compressibility[" "]
        isq_isentropic_compressibility["<b><a href="../../systems/isq/#isentropic_compressibility" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::isentropic_compressibility</a></b><br><i>(inverse(<a href="../../systems/isq/#volume" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">volume</a>) * (<a href="../../systems/isq/#volume" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">volume</a> / <a href="../../systems/isq/#pressure" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">pressure</a>))</i>"]
    end
    subgraph kind_isq_isothermal_compressibility[" "]
        isq_isothermal_compressibility["<b><a href="../../systems/isq/#isothermal_compressibility" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::isothermal_compressibility</a></b><br><i>(inverse(<a href="../../systems/isq/#volume" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">volume</a>) * (<a href="../../systems/isq/#volume" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">volume</a> / <a href="../../systems/isq/#pressure" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">pressure</a>))</i>"]
    end
```

**Legend:**

- Subgraphs with a dotted line from the parent indicate a distinct quantity kind (created with `is_kind`). These subtrees are type-isolated: quantities inside cannot be added or compared to those outside their subgraph without explicit conversion.
