# Computational String Art

## To compute the size of the frame

given w, h of image, N nails

```
N += N % 2
n = (int) (r / (r+1)) * (N / 2)
r = min( abs(2*n / (N - 2*n) - (w / h)) < abs(2*(n+1) / (N - 2*(n+1)) - (w / h)) )

number of nails on base side(including both corners) = n + 1
number of nails on height side(including both corners) = N / 2 - n + 1

if (w>h) {
    new_w = w
    new_h = new_w / r
} else {
    new_h = h
    new_w = new_h * r
}

spacing = w / n
```

## To fit image to calculated width and height by center cropping

```
todo
```
