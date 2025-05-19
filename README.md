# Computational String Art

## To compute the size of the frame

given w, h of image, N nails

```{c}
N += N % 2
n = (int) (r / (r+1)) * (N / 2)
r0 = min( abs(2*n / (N - 2*n) - (w / h)) < abs(2*(n+1) / (N - 2*(n+1)) - (w / h)) ) + r

// number of nails on base side(including both corners) = n + 1
// number of nails on height side(including both corners) = N / 2 - n + 1

if (r0>r) {
    new_w = w
    new_h = new_w / r
} else {
    new_h = h
    new_w = new_h * r
}

spacing = new_w / n
```

## To fit image to calculated width and height by center cropping

```{c}
int rowStart = floor((double)(imageData->height - newHeight) / 2.);
int colStart = floor((double)(imageData->width - newWidth) / 2.);
for (int rowi=rowStart; rowi <= rowStart + newHeight; ++rowi ) {
    for (int coli=colStart; coli <= colStart + newWidth; ++coli ) {
        for (int color=0; color<imageData->numColorChannels; ++color) {
            image->data[((rowi - rowStart) * newWidth + coli - colStart) * imageData->numColorChannels + color] =
            imageData->data[((rowi * imageData->width) + coli) * imageData->numColorChannels + color];
        }
    }
}
```
