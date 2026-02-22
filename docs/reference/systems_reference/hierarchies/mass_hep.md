<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# mass Hierarchy

**System:** HEP

```mermaid
flowchart LR
    hep_mass["<b><a href="../../systems/hep/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::mass</a></b><br><i>(<a href="../../systems/hep/#energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">energy</a> * pow<2>(<a href="../../systems/hep/#duration" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">duration</a>) / pow<2>(<a href="../../systems/hep/#length" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">length</a>))</i>"]
    hep_effective_mass["<b><a href="../../systems/hep/#effective_mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::effective_mass</a></b>"]
    hep_mass --- hep_effective_mass
    hep_invariant_mass["<b><a href="../../systems/hep/#invariant_mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::invariant_mass</a></b>"]
    hep_mass --- hep_invariant_mass
    hep_reduced_mass["<b><a href="../../systems/hep/#reduced_mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::reduced_mass</a></b>"]
    hep_mass --- hep_reduced_mass
    hep_rest_mass["<b><a href="../../systems/hep/#rest_mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::rest_mass</a></b>"]
    hep_mass --- hep_rest_mass
```
