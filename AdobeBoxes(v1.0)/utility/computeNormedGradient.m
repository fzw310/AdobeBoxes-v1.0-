function mag1f = computeNormedGradient( im3u, flag)

im3f = double(im3u);

[gx1, gy1] = computeGradient(im3f(:, :, 1));
[gx2, gy2] = computeGradient(im3f(:, :, 2));
[gx3, gy3] = computeGradient(im3f(:, :, 3));

gx = max(abs(cat(3, gx1, gx2, gx3)), [], 3);
gy = max(abs(cat(3, gy1, gy2, gy3)), [], 3);

if flag == 3
    mag1f = min((gx + gy), 255);
else
    if flag == 2
        mag1f = min(gy, 255);
    else
        mag1f = min(gx, 255);
    end
end
end

function [gx, gy] = computeGradient( im1f )
%COMPUTEGRADIENTMAG computes gradient magnitude of image
%

gx = zeros(size(im1f), class(im1f));
gy = zeros(size(im1f), class(im1f));

gx(:,2:end-1) = conv2(im1f, [1 0 -1], 'valid');
gy(2:end-1,:) = conv2(im1f, [1;0;-1], 'valid');

% forward difference on borders
gx(:,1)   = (im1f(:,2)   - im1f(:,1)) * 2;
gx(:,end) = (im1f(:,end) - im1f(:,end-1)) * 2;

gy(1,:)   = (im1f(2,:)   - im1f(1,:)) * 2;
gy(end,:) = (im1f(end,:) - im1f(end-1,:)) * 2;

end