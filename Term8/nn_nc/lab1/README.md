Script to rename images (execute in directory with images):

```
tmp=(*); for i in ${!tmp[*]}; do mv ${tmp[$i]} $i.png; done
```

