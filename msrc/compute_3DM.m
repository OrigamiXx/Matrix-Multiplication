function c = compute_3DM(p, tight)


    dims = size(p);
    s = dims(1);
    k = dims(2);

    c = logical(zeros(s,s,s));
    
    for i = 1:s
        for j = 1:s
            for l = 1:s
                for m = 1:k
                    t = (p(i,m) == 1) + (p(j,m) == 2) + (p(l,m) == 3);
                    if t == 2
                        c(i,j,l) = true;
                        break;
                    end
                end
            end     
        end
    end
    
    if tight 
        for m = 1:k
           counts = zeros(1,3);
           for i = 1:s 
                counts(p(i,m)) = counts(p(i,m)) + 1;
           end
           if any(counts == 0)
               fprintf('column %d is special\n',m);
               for i = 1:s
                   for j = 1:s
                       if p(i,m) ~= p(j,m)
                           for l = 1:s
                               if counts(1) == 0
                                   c(l,i,j) = true;
                                   c(l,j,i) = true;
                               elseif counts(2) == 0
                                   c(i,l,j) = true;
                                   c(j,l,i) = true;
                               else
                                   c(i,j,l) = true;
                                   c(j,i,l) = true;
                               end
                           end
                       end
                   end
               end
           end
        end
    end
    
    c = ~c;



end